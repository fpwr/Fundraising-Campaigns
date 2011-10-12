/* heavily based upon Taylors FPWR javascript */

var dw_campaigns = {
	
	/**
	 * Called on every page load
	 */	
	init: function() {
		//handle any default text fields
		$('.has-default-text').hasDefaultText();
	},
        /**
         * Handles the map in any blocks
         */
        eventMaps:              {},
        geocoder:               null,
        infoWindows:    [],
        initEventsMap: function(params) {
           
                if(this.geocoder == null) {
                        this.geocoder = new google.maps.Geocoder();
                }

                if(!params.zoom) {
                        params.zoom = 11;
                }

                var latlng = new google.maps.LatLng(params.lat, params.long);
                var myOptions = {
                                zoom: params.zoom,
                                center: latlng,
                                mapTypeId: google.maps.MapTypeId.ROADMAP,
                                mapTypeControl: false
                };
                var container = $(params.container)[0];
                if(container.id == '') {
                        alert("You must specify an id for your map.");
                }

                this.eventMaps[container.id] = new google.maps.Map(container, myOptions);
        },

        addMarkerToMap: function(container, location, title, html) {

                var me          = this;
                var map         = me.eventMaps[$(container)[0].id];

                var marker = new google.maps.Marker({
                        map:            map, 
                        position:       new google.maps.LatLng(location.lat,location.lng),
                        title:          title
                });

                var infowindow = new google.maps.InfoWindow({
                    content: html
                });

                google.maps.event.addListener(marker, 'click', function() {
                        for(var c=0; c<me.infoWindows.length; c++) {//var index in me.infoWindows) {
                                me.infoWindows[c].close();
                        }

                        infowindow.open(map,marker);
                });

                me.infoWindows.push(infowindow);

        },	
	/**
	 * will refresh states when a country is selected
	 */
	selectedState: null,
	initCountrySelect: function(countrySelect, stateSelect) {
		var me = this;
		$(countrySelect).change(function() {
			
			//remove all state/province options
			stateSelect = $(stateSelect)[0];
			
			//track the selected state so if we are simply refreshing we keep the same one selected
			if(me.selectedState == null) {
				me.selectedState = $(stateSelect).val();
			}
		
                        if(typeof stateSelect.options == 'undefined') {
				return;
			}
	
			//remove all options
			for(var index = stateSelect.options.length - 1; index >= 0; index --) {
				stateSelect.remove(index);
			}
			
			//drop in temp loading option
			var option 		= document.createElement('option');
			option.text 	= "Loading...";
			option.value	= -1;
			stateSelect.options.add(option);
			stateSelect.disabled = true;
			
			$.ajax({
				dataType: 'json',
				url: '/dw/ajax/statesQuery/' + $(this).val(),
				success: function(results) {
					
					//remove the loading optoins
					stateSelect.remove(0);
					
					//add in new options
					for(var index = 0; index < results.length; index++) {
						var option 		= document.createElement('option');
						option.text 	= results[index].name;
						option.value	= results[index].value;
						stateSelect.options.add(option);
					}
					
					//re-select the selected state if one is set
					if(me.selectedState > 0) {
						$(stateSelect).val(me.selectedState);
					}
					
					stateSelect.disabled = false;
				}
			});
			
		});
		
		$(countrySelect).change();
	},
    
	initColorPickers: function() {
		$('.color1-picker').ColorPicker({
			flat:       true,
			color: $("#edit-color1").val(),
			onChange: function(hsb, hex, rgb, el) {
				$("#edit-color1").val(hex);
				$(".team-logo .inner").css('background-color','#' + hex);
			}
		});
		
		$('.color2-picker').ColorPicker({
			flat: true,
			color: $("#edit-color2").val(),
			onChange: function(hsb, hex, rgb, el) {
				$("#edit-color2").val(hex);
				$(".team-logo").css('background-color','#' + hex);
			}
		});
	},
	personalTypedInFlag: null,
	initIntroText: function(characterCount, introText) {
		var me = this;
		$(introText).keydown(function(event) {

			var text = $(this).val();
			var length = text.length;
			var code = (event.keyCode ? event.keyCode : event.which);

			// let movement and delete chars thru always
			if(length >= __MAX_CHARS__) {
				if( code == 8 || code == 46 || code == 38 || code == 39 || code == 40 || code == 41) {
					return true;
				}
				return false;
			}
			return true;
		});

                $(introText).keyup(function() {
                        if($(this).val().length > __MAX_CHARS__) {
                                $(introText).val($(introText).val().substr(0, __MAX_CHARS__));
                        }
                        $(characterCount).html($(this).val().length + ' / __MAX_CHARS__ characters used');
                });

		// fire once to init
		$(introText).keydown();
		$(introText).keyup();

	},
	initLivePosition: function(pcp_id, amountSource, destination) {
		var request = null;
		$(amountSource).keyup(function() {

			var amount = $(amountSource).val();
			if(amount.length == 0) {
                            amount = '0';
                        }
                        if(request) {
                            request.abort();
                        }

                        request = $.get('/dw/ajax/position/' + pcp_id + '/' + amount, function(data) {
                            $(destination).html(data);
                        });

		});
		$(amountSource).keyup();
	},
	initRegistrationLocation: function(locationField, target, emptyWords) {
		var request = null;
		$(locationField).change(function() {
                        if(request) {
                            request.abort();
                        }

			var location_id = $(locationField).val();
			if(location_id.length == 0) {
				$(target).hide();
				$(emptyWords).show();
                        } else {
                        	request =	$.get('/dw/ajax/registrationlocation/' + location_id, 
							function(data) {
								$(emptyWords).hide();
                            					$(target).html(data); 
								$(target).show();
							}
						);
			}

		});
		$(locationField).change();
	}
};

/**
 * Plugin to require unique username before a form is submitted
 */
(function($) {
	
	$.fn.hasDefaultText = function(){
		return this.each(function(){
			var default_value = $(this).val();
			$(this).focus(function(){
				if ($(this).val() == default_value) {
					$(this).val("");
					$(this).addClass('has-focus');
				}
			});
			$(this).blur(function(){
				if ($(this).val() == "") {
					$(this).val(default_value);
					$(this).removeClass('has-focus');
				}
				
			});
		});
	};
	
	$.fn.extend({
		
		requireUniqueUsername: function() {
			this.each(function() {
				
				var input = $(this);
				
				/**
				 * Wrap div in container to help place progress
				 */
				var wrapper = $(input).wrap('<div class="username-unique-wrapper"></div>');
				
				/**
				 * Drop in progress gif div first so we can hide or show
				 */
				var progress = $('<div class="username-unique-progress"></div>').insertAfter(input);
				$(progress).hide();
				
				/**
				 * Drop in results div
				 */
				var resultsContainer = $('<div class="username-unique-results"></div>').insertAfter($(input).parent("div:first"));
				
				
				/**
				 * search function
				 */
				var request = null;
				var search = function(username) {
					
					/**
					 * Stop any previously running reqests
					 */
					if(request) {
						request.abort();
					}
					
					$(progress).show();
					
					/**
					 * Search
					 */
					request = $.ajax({
						url: '/dw/ajax/username-search',
						type: 'post',
						data: 'username=' + username,
						success: function(results) {

							results_split = results.split('|', 2);

							$(input).val(results_split[1]); // set safe value
							$('.username_insert').html(results_split[1]);
							$(progress).hide();
							$(resultsContainer).removeClass('username-found').removeClass('username-not-found');
							
							/**
							 * Do not let them select a username we found from our search
							 */
							if(results_split[0] == 'found') {
								$(resultsContainer).addClass('username-found').html("Personal URL is already taken.");
								disableSubmit();
							} else {
								$(resultsContainer).addClass('username-not-found').html("Personal URL is available.");
								enableSubmit();
							}
						}
					});
				};
				
				var submitButton 	= $(input).parents("form:first").find("input[type='submit']");
				var submitEnabled 	= false;
				var disableSubmit = function() {
					$(submitButton).css('opacity',0.5);
					submitEnabled = false;
				};
				
				var enableSubmit = function() {
					$(submitButton).css('opacity',1);
					submitEnabled = true;
				};
				
				$(input).parents("form:first").submit(function() {
					return submitEnabled;
				});
				
				/**
				 * Attach listener to actual input
				 */
				$(input).keyup(function() {
					search($(this).val());
				});
				
				/**
				 * Disable submit button to start
				 */
				disableSubmit();
				
				/**
				 * if the username field has a value, verify it
				 */
				if($(input).val().length > 0) {
					search($(input).val());
				}
				
			});
		},
		assignPersonalURL: function() {
			this.each(function() {
				
				var input = $(this);
				
				/**
				 * search function
				 */
				var request = null;
				var convert = function(userinput) {
					if(dw_campaigns.personalTypedIn) {
						return;
					}
					
					/**
					 * Stop any previously running reqests
					 */
					if(request) {
						request.abort();
					}
					
					/**
					 * Search
					 */
					request = $.ajax({
						url: '/dw/ajax/makeURL',
						type: 'post',
						data: 'name=' + userinput,
						success: function(results) {
							$('#edit-username').val(results);
							$('#edit-username').keyup();
						}
					});
				};
				
				
				/**
				 * Attach listener to actual input
				 */
				$(input).keyup(function() {
					convert($(this).val());
				});
				

				if($(input).val().length > 0) {
					convert($(input).val());
				}
				
			});
		}
		
	});
	
	
	
}(jQuery));

$(document).ready(function(){ 
	dw_campaigns.init();

	// debugging
	// $("#show-words").fancybox().trigger("click");	

        $("#show-words").fancybox({
		transitionIn: 'elastic',
		transitionOut: 'elastic',
		type : 'inline',
		height: 480,
		width: 480
	});
        $("a.fb").fancybox({
		transitionIn: 'elastic',
		transitionOut: 'elastic',
		type : 'iframe',
		height: 480
	});
        $("a.fb_tall").fancybox({
		transitionIn: 'elastic',
		transitionOut: 'elastic',
		type : 'iframe',
		height: 700
	});
});
