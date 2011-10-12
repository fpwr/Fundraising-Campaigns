<?php
    if(!isset($campaign) || is_null($campaign)) { 
        $campaign           = $node; // its named differently in the event block for some reason
    }

    $address            = dw_campaigns_build_location_addr($campaign->field_dw_address[0]);
    $campaignLocation   = dw_campaigns_get_location_for_address($address);
    $location           = $campaignLocation;
    $title              = addslashes($campaign->title);
    $html               = json_encode('<div class="map-infowindow"><h3>' . $title . "</h3></div>");

    $temp_addr = preg_replace('/[\ ,]/', '', $address);
    if(empty($temp_addr)) {
        $event_map = '<div id="events-map-wrapper"><div id="events-map">No address available</div></div>';
        return;       
    }

    $event_map = '
<div id="events-map-wrapper">
        <div id="events-map"></div>
</div>';
?>
<script type="text/javascript" src="http://maps.google.com/maps/api/js?sensor=false"></script>
<script type="text/javascript">
$(document).ready(function() {

    var mapContainer = $('#events-map');


<?php
if(!isset($location['lat'])) {
?>
    alert('invalid location: ' + '<?php echo htmlentities($address); ?>');
});
    </script>
<?php
    return;
}
?>

    dw_campaigns.initEventsMap({
        container: mapContainer,
        zoom: 13,
        lat: <?php echo $location['lat']; ?>,
        long: <?php echo $location['lng']; ?>
    });

    dw_campaigns.addMarkerToMap(mapContainer,<?php echo json_encode($campaignLocation); ?>,"<?php echo $title; ?>",<?php echo $html; ?>); 
});

</script>
