<?php

function dw_campaigns_user_register_oss_page1_page() {
    $form = drupal_get_form('dw_campaigns_user_register_oss_page1_form');
    return theme('dw_campaigns_user_register_oss_page1_page_theme', $form);
}


function dw_campaigns_user_register_oss_page1_form() {
    
    global $user;

    _user_password_dynamic_validation();
    $contact    = dw_campaigns_resolve_contact($user);


    $form = array();
    $form['#attributes']    = array(
        'class'             => 'dw_campaigns-profile-form',
        'enctype'           => "multipart/form-data"
    );

    $locations          = dw_campaigns_get_all_campaign_location();
    $location_string    = '';

    $selected           = dw_campaigns_get_selected_location();
    
    if(is_null($selected)) {
        $selected = -1;
    } else {
        if(isset($locations[$selected])) {
            $campaign       = node_load($selected);
            dw_campaigns_set_selected_campaign($campaign);
            $found_location = TRUE;
        } else {
            if($selected != 0) {
                echo "invalid node ($selected)";
            }	
            dw_campaigns_set_selected_campaign(NULL);
        }
    }
    if(!$found_location) {
        $locations = array('0' => 'Select Location') + $locations;
    }

    $form['location'] = array(
        '#type'             => 'select',
        '#title'            => t('Event Location'),
        '#options'          => $locations,
        '#default_value'    => $selected,
        '#weight'           => -1,
        '#required'         => true
    );

    $address_form   = dw_campaigns_address_form_fields($contact);
    $form           = array_merge($form, $address_form);
    unset($form['address_2']);
/*
    $form['free_blurb'] = array(
        '#type'              => 'markup',
        '#value'             => '<p>' . t('Receive a FREE t-shirt and entry to one of many prizes when you raise a minimum of $40 or more! All children (under 16) receive a free t-shirt.') . '</p><h2>' . t('Participation Type') . '</h2>'
    );
*/
    $participation_type_words = '';
    if(!is_null($campaign)) {
        $participation_type_words = $campaign->field_dw_part_type_blurb[0]['value'];
        if(empty($participation_type_words)) {
            $participation_type_words = variable_get('dw_campaigns_participation_type_blurb', ''); 
        }
    }

    $form['free_blurb'] = array(
        '#type'              => 'markup',
        '#value'             => '<h2 class="participation-type">' . t('Participation Type') . '</h2>'
    );
    //$participation_options = dw_campaigns_get_oss_participation_type($campaign);
    $participation_options = dw_campaigns_get_oss_participation_type(NULL);
 
    $form['participation_type'] = array(
        '#type'             => 'radios',
        '#title'            => t('Participation Type'),
        '#options'          => $participation_options,
        '#attributes'       => array('class'=> 'participation_type'),
	'#prefix'           => '<div class="participation-type-wrapper hidden">' . '<div class="participation_type_wds">' . $participation_type_words . '</div>',
	'#suffix'           => '</div>',
        '#required'         => true
    );
    $form['participation_blurb'] = array(
        '#type'    => 'markup',
        '#value'   => '<div id="participation_blurb">Please Select a Location</div>'
    );

    drupal_add_js('$(document).ready(function() {
	dw_campaigns.initRegistrationLocation("#edit-location", ".participation-type-wrapper", "#participation_blurb");
    });', 'inline');



    $form['button-break'] = array(
        '#type'    => 'markup',
        '#value'   => '<div class="button-break"></div>'
    );
    $form['cancel'] = array(
        '#type'             => 'markup',
        '#value'            => '<a onclick="location.replace(&quot;/dw/cancel/register&quot;); return false;" class="form-submit btn" value="Cancel" id="edit-cancel" name="op">Cancel</a>'
    );
    
    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> 'btn')
    );

    $mode_type   = dw_campaigns_get_selected_type();

    if($mode_type == 'walking' && arg(2) == 'register_oss') {
	$form['submit']['#value'] = t('Next');
    }
    return $form;
}

function dw_campaigns_get_oss_participation_type_ajax($location_id) {
    $form = drupal_get_form('dw_campaigns_get_oss_participation_type_ajax_form', $location_id);
    $form = preg_replace('_\<[\/]?form.*?\>_', '', $form);
    $form = preg_replace('_\<input type="hidden".*?\>_', '', $form);

    echo $form;
    die;
}

function dw_campaigns_get_oss_participation_type_ajax_form($form, $location_id) {
    $form = array();

    
    $campaign = node_load($location_id);
    if(!$campaign || is_null($campaign)) {
	echo t('<span class="no-location">Please select an event location at the top of this form</span>');
        die;
    }

    $participation_type_words = '';

    $participation_type_words = $campaign->field_dw_part_type_blurb[0]['value'];
    if(empty($participation_type_words)) {
        $participation_type_words = variable_get('dw_campaigns_participation_type_blurb', '');
    }

    $participation_options = dw_campaigns_get_oss_participation_type($campaign);

    $form['participation_type'] = array(
        '#type'             => 'radios',
        '#title'            => t('Participation Type'),
        '#options'          => $participation_options,
        '#attributes'       => array('class'=> 'participation_type'),
        '#prefix'           => '<div class="participation_type_wds">' . $participation_type_words . '</div>',
        '#required'         => true
    );

    return $form;
}

function dw_campaigns_get_oss_participation_type($campaign) {

    $campaign = (is_object($campaign)) ? $campaign : node_load($campaign);

// since I changed the above to always try to load a node (even NULL) $campaign will be false

    if(is_null($campaign) || !$campaign) {
        $adult_pledge     = '';
        $adult_purchase   = '';
        $family_pledge    = ''; 
        $runner_pledge    = ''; 
        $runner_purchase  = ''; 
    } else {
        $a = dw_campaigns_get_oss_participation_prices($campaign);
        extract($a);
    }

    $types = array();

    if($adult_pledge != -1) {    
        $types['adult_pledge']    = t('<span class="participation-label">Adult - Commit to raise $!amount</span> ......... <span class="option_price">FREE</span><span class="next_line">Commit to raise $!amount per adult and register for free!</span>', array('!amount' => $adult_pledge));
    }
    if($adult_purchase != -1) {
        $types['adult_purchase']  = t('<span class="participation-label">Adult - $!amount Registration Fee</span> ......... <span class="option_price">$!amount</span><span class="next_line">The fee is not tax-receiptable and non-refundable</span>', array('!amount' => $adult_purchase));
    }
    if($family_pledge != -1) {
        $types['family_pledge']   = t('<span class="participation-label">Family - Commit to raise $!amount</span> ......... <span class="option_price">FREE</span><span class="next_line">(2 adults plus children) Commit to raise $!amount per family and register for free!</span>', array('!amount' => $family_pledge));
    }
    if($runner_pledge != -1) {
        $types['runner_pledge']   = t('<span class="participation-label">Runner - Commit to raise $!amount</span> ......... <span class="option_price">FREE</span><span class="next_line">Commit to raise $!amount per adult and register for free!</span>', array('!amount' => $runner_pledge));
    }
    if($runner_purchase != -1) {
        $types['runner_purchase'] = t('<span class="participation-label">Runner - $!amount Registration Fee</span> ......... <span class="option_price">$!amount</span><span class="next_line">The fee is not tax-receiptable and non-refundable</span>', array('!amount' => $runner_purchase));
    }

    return $types;
}

function _dw_campaigns_validate_oss_participation_type($campaign, $type) {
    $types = dw_campaigns_get_oss_participation_type($campaign);
    if(isset($types[$type])) {
        return true;
    } else {
        return false;
    }
}

function dw_campaigns_get_oss_participation_prices($campaign) {

    $a = array();

    $a['adult_pledge']     = !is_null($campaign->field_dw_part_adult_pledge[0]['value']) ? $campaign->field_dw_part_adult_pledge[0]['value'] : 40; 
    $a['adult_purchase']   = !is_null($campaign->field_dw_part_adult_purchase[0]['value']) ? $campaign->field_dw_part_adult_purchase[0]['value'] : -1;
    $a['family_pledge']    = !is_null($campaign->field_dw_part_family_pledge[0]['value']) ? $campaign->field_dw_part_family_pledge[0]['value'] : -1;
    $a['runner_pledge']    = !is_null($campaign->field_dw_part_runner_pledge[0]['value']) ? $campaign->field_dw_part_runner_pledge[0]['value'] : -1;
    $a['runner_purchase']  = !is_null($campaign->field_dw_part_runner_purchase[0]['value']) ? $campaign->field_dw_part_runner_purchase[0]['value'] : -1;

    return $a;

}

function dw_campaigns_user_register_oss_page1_form_validate(&$form, &$form_state) {

    if($form_state['clicked_button']['#submit'][0] == 'dw_campaigns_form_cancel') {
        dw_campaigns_form_cancel('register');
        return;
    }

    $values = $form_state['values'];
    if(isset($values['location']) && $values['location'] == '0') {
        form_set_error('location',t('You must select a location'));
    }
    
    //check email
    if(!dw_campaigns_user_is_email_unique($values['email'])) {
        form_set_error('email',t('The email you entered was already taken.'));
    }

    if(!dw_campaigns_user_is_email_unique_civi($values['email'])) {
        form_set_error('email',t('The email you entered was already taken in civi.'));
    }

    if(!_dw_campaigns_validate_oss_participation_type($values['location'], $values['participation_type'])) {
        form_set_error('participation_type', t('Invalid Participation Type, please start the registration process over'));
    }
	
}

function dw_campaigns_user_register_oss_page1_form_submit(&$form, &$form_state) {
    $values     = $form_state['values'];
    $locationId = $values['location'];

    $_SESSION['last_form'] = $values;
    if(strstr($values['participation_type'], "_pledge") !== FALSE) {
        drupal_goto('dw/user/register_oss_pcp/' . $locationId);
    } else {
        drupal_goto('dw/user/register_oss_payment/' . $locationId);
    }
}

function dw_campaigns_user_register_oss_create_pcp_page($campaignId) {
    global $user;

    $contactId  = _dw_campaigns_user_get_contact_id($user);

    if(is_null($contactId)) {
    }

    if($campaignId == 'create_page') {
        $campaignId = -1;
    }

    //get the registration form, but in simplified form
    $form = drupal_get_form('dw_campaigns_user_register_oss_create_pcp_page_form', $campaignId);
    return  theme('dw_campaigns_user_create_pcp_page_theme', $campaign, $form);

}

function dw_campaigns_user_register_oss_create_pcp_page_form($form, $campaignId) {
    global $user;
    $form = array();

    $mode_type   = dw_campaigns_get_selected_type();

    if($campaignId != -1) {
        $campaign = node_load($campaignId);
    }

    if($campaignId != -1 && (is_null($campaign->nid) || !dw_campaigns_valid_type($campaign))) {
            drupal_set_message("Unable to create Personal Campaign Page");
            drupal_goto('dw');
            return;
    }

    $in_form = dw_campaigns_user_register_form('pcp-photo', $pcp->id, NULL, FALSE);

    $form = array_merge($form, $in_form);
    
   
    $form['header-description'] = array(
        '#type'             => 'markup',
        '#value'            => t('<h2>Create your Fundraising Page</h2><p>and support the !title</p>', array('!title' => $campaign->title)),
	'#weight'           => -999
    );
        
    $form['verify-heading'] = array(
        '#type'             => 'markup',
        '#value'            => '<h3>' . t('Verify Your Personal Details') . '</h3>',
        '#weight'           => -99
    );

    $form['participation_type'] = array(
        '#type'             => 'hidden',
        '#default_value'    => $_SESSION['last_form']['participation_type']
    );

    if(isset($_SESSION['last_form'])) { 

        $form['first_name']['#default_value']  = $_SESSION['last_form']['first_name'];
        $form['last_name']['#default_value']   = $_SESSION['last_form']['last_name']; 
        $form['address_1']['#default_value']   = $_SESSION['last_form']['address_1'];
        $form['address_2']['#default_value']   = $_SESSION['last_form']['address_2'];
        $form['city']['#default_value']        = $_SESSION['last_form']['city'];
        $form['state']['#default_value']       = $_SESSION['last_form']['state'];
        $form['postal_code']['#default_value'] = $_SESSION['last_form']['postal_code'];
        $form['country']['#default_value']     = $_SESSION['last_form']['country'];
        $form['email']['#default_value']       = $_SESSION['last_form']['email'];
    
        unset($form['header-description']);
        unset($form['verify-heading']);

        //unset($_SESSION['last_form']);

        $form['first_name']['#type'] = 'hidden';
        $form['last_name']['#type'] = 'hidden';
        $form['address_1']['#type'] = 'hidden';
        $form['address_2']['#type'] = 'hidden';
        $form['city']['#type'] = 'hidden';
        $form['state']['#type'] = 'hidden';
        $form['postal_code']['#type'] = 'hidden';
        $form['country']['#type'] = 'hidden';
        $form['email']['#type'] = 'hidden';
        $form['telephone']['#type'] = 'hidden';

    }



    unset($form['address_2']);

    drupal_add_js('$(document).ready(function() {
        $("#edit-displayname").assignPersonalURL();
        $("#edit-username").keydown(function() {
            dw_campaigns.personalTypedIn    =   true;
        });
    });', 'inline');


    // lookup picture
    $temp_filename='';
    $temp_file=_dw_campaigns_user_upload_photo();
    if(isset($temp_file->filepath)) {
	$temp_filename=$temp_file->filepath;
    }

    $image_params = array(
        'w'                 => 100,
        'src'               => $temp_filename
    );

    $matched_image = '';
    //get the user and drop it in for a preview
    $imageSrc = _dw_campaigns_get_photo($user,  $image_params, $type, $resource_id, $matched_image);

    $form['picture-heading']['#value'] = '<h3 class="clear">' . t('Personal Campaign Picture') . '</h3>';

    $form['user-photo-tmp'] = array(
        '#type'             => 'hidden',
        '#value'            => $temp_filename
    );
 
    $form['picture-preview'] = array(
        '#type'             => 'markup',
        '#value'            => '<div id="profile-form-image-wrapper"><img src="' . $imageSrc . '" /></div>'
    );
	
    $form['user-photo']     = array(
        '#type'             => 'file',
        '#title'            => t('Select A Photo'),
        '#attributes'       => array(
            'size'          => ''
        )
    );
    
    $form['picture-description'] = array(
        '#type'             => 'markup',
        '#value'            => '<div id="profile-form-image-description" class="form-item">' . t('This image will be displayed on your personal campaign page.  You must select a file that ends in .jpg, .jpeg, .gif, or .png. Note: The file is not uploaded until you hit "Submit" below.') . '</div>'
    );


    $form['youtube-description'] = array(
        '#type'             => 'markup',
        '#value'            => '<div id="youtube-description">' . t('As an alternative to supplying a photo, you may supply a link to a youtube video which will be used in place of your photo on your Personal Campaign Page') . '</div>',
    );

    $form['youtube_url'] = array(
        '#type'             => 'textfield',
        '#title'            => 'YouTube Video URL',
        '#default_value'    => $extra->youtube_url,
        '#description'      => t('To add a Youtube video, go to www.YouTube.com, click the "share" button of the video you would like to add and copy the "Link to this video".'),
        '#required'         => false 
    );
    
    //drop in fields so user can fill out details about the event
    $form['contribution-heading'] = array(
        '#type'             => 'markup',
        '#value'            => '<h3>' . t('Your Campaign Details') . '</h3>',
        '#weight'           => -999
    );
	
    if(!isset($pcp->id) || is_null($pcp->id)) {
        $pcp->goal_amount   = NULL;
        $pcp->intro_text    = NULL;
        $pcp->title         = NULL;
    }

    if(empty($user->displayname)) {
        $title = !empty($pcp->title)?$pcp->title:$_SESSION['last_form']['first_name'] . " " . $_SESSION['last_form']['last_name'] . "'s " . $campaign->title . ' Page';
    } else {
        $title = !empty($pcp->title)?$pcp->title:$user->displayname . "'s " . $campaign->title . ' Page';
    }

    $form['contribution-heading']['title'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Page Title'),
        '#default_value'    => $title,
        '#required'         => true 
    );    


    $goal_amount = variable_get('dw_campaign_default_pcp_goal', '100.00');

    if(!is_null($pcp->goal_amount)) {
        $goal_amount = $pcp->goal_amount;
    }

    $form['contribution-heading']['goal_amount'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Fundraising Goal'),
        '#default_value'    => $goal_amount,
        '#required'         => true 
    );

    $event_max = variable_get('dw_campaigns_walking_event_max', '6');

    for($i=0; $i<=$event_max; $i++) {
        $attending[$i] = $i;
    }


    $default_value = dw_campaigns_get_event_participants_count($campaignId, $pcp->id);
    if($default_value == 0) {
        $default_value     = 1;
        if(strstr($_SESSION['last_form']['participation_type'], "family") !== FALSE) {
            $default_value = 2;
        }
    }


    if(strstr($_SESSION['last_form']['participation_type'], "runner") !== FALSE) {
        $form['contribution-heading']['num_attending'] = array(
            '#type'             => 'hidden',
            '#value'    => $default_value
        );
    } else { 
        $form['contribution-heading']['num_attending'] = array(
            '#type'             => 'select',
            '#title'            => t('How many people will be attending the event'),
            '#options'          => $attending,
            '#default_value'    => $default_value
        );
    }


    if($mode_type == 'walking') {
   
        $event_help_text = variable_get('dw_campaigns_walking_event_help_text', '');

        if(is_null($pcp->id)) {
            $tos_words = $campaign->field_dw_event_tos[0]['value'];

            if($language->language == 'fr') {
	        $tos_words = 'Conditions Dans le cadre de ma participation volontaire àa marche One SMALL Step 2011, jaccepte par la prénte de participer àes propres risques. Jassume tous les risques pour moi-mê et mes biens, peu importe la nature de laccident, et je libè FPWR et FPWR Canada de toutes responsabilité Je les prérve contre toutes actions, réamations ou demandes de toutes natures, maintenant ou àimporte quel moment dans le futur, de ma propre personne, mes hétiers, mes exéteurs testamentaires, mes administrateurs ou de mes cessionnaires contre les commanditaires de la marche One SMALL Step 2011 concernant toute perte, blessure ou tout dommage causéar une néigence, une erreur ou une mauvaise conduite des commanditaires ou de leurs employé mandataires ou membres, de quelque faç que ce soit. a';
            }

            $form['contribution-heading']['tos_words'] = array(
                '#type'             => 'markup',
                '#value'            => '<div class="tos_words">' . $tos_words . '</div>'
            );
            
            $form['contribution-heading']['tos_checkbox'] = array(
                '#type'             => 'checkbox',
                '#description'      => t('I have read and accept these terms and conditions.')
            );
        }

    }

    $max_chars = dw_campaign_get_max_intro();

    $form['contribution-heading']['words_used'] = array(
        '#type'             => 'markup',
        '#value'            => '<div class="screen-name-description">' . t('Write a brief paragraph describing your motivation for participating.  This text is shown on your custom web page.') . '</div><div class="character_counter">' . t('!chars characters used', array('!chars' => '0 / ' . $max_chars)) . '</div>',
    );

    $default_intro_text = $campaign->field_dw_pcp_intro_text[0]['value'];
     
    
    if(!empty($default_intro_text)) {
        $contactId      = _dw_campaigns_user_get_contact_id($user);
        $mergeObjects   = dw_campaigns_get_merge_object($campaign, NULL, $contactId);
        dw_campaigns_do_merge($default_intro_text, $mergeObjects);
    }
    
    $intro_text = !empty($pcp->intro_text) ? $pcp->intro_text : $default_intro_text;
    
    $intro_text = substr($intro_text, 0, $max_chars); // truncate
    
    $form['contribution-heading']['intro_text'] = array(
        '#type'             => 'textarea',
        '#title'            => 'Description',
        '#required'         => true,
        '#default_value'    => $intro_text,
        '#attributes'       => array('class'=> 'intro_text')
    );


    drupal_add_js('$(document).ready(function() {
        dw_campaigns.initIntroText(".character_counter", "#edit-intro-text");
    });','inline');
  
 
    $form['campaign-id'] = array(
            '#type'             => 'hidden',
            '#default_value'    => $campaignId
    );

    $form['cancel'] = array(
        '#type'             => 'markup',
        '#value'            => '<a onclick="location.replace(&quot;/dw/cancel/pcp&quot;); return false;" class="form-submit btn" value="Cancel" id="edit-cancel" name="op">Cancel</a>',
        '#weight'           => 998
    );
    
    //change label on submit button so people know they are creating a contribution page
    $form['submit']['#value']    = t("Create My Page");
    $form['submit']['#weight']   = 999;

    return $form;
}




/**
 * Validates the form in the create pcp page process
 * 
 * @param array $form
 * @param array $form_state
 */
function dw_campaigns_user_register_oss_create_pcp_page_form_validate(&$form, &$form_state) {

    global $user;
 
    // we called this different things on different forms, lets normalize it (and make sure both are set)
    // step 1, make sure location is set
    $form_state['values']['location'] = isset($form_state['values']['location']) ? $form_state['values']['location'] : $form_state['values']['campaign-id'];
    $form_state['values']['campaign-id'] = $form_state['values']['location'];

    // step 2
    if($form_state['values']['location'] == 0) {
        form_set_error('location', t('You must select a location'));
    } else {
        // do nothing special
    }


    $values = $form_state['values'];
    $mode_type   = dw_campaigns_get_selected_type();


    if(isset($values['tos_checkbox']) && $values['tos_checkbox'] != 1) {
        form_set_error(']event-fields][tos_checkbox', 'You must agree to the waiver');
        return;
    }

/// START
    if($form_state['clicked_button']['#submit'][0] == 'dw_campaigns_form_cancel') {
        dw_campaigns_form_cancel('register');
        return;
    }

    $values = $form_state['values'];
    if(isset($values['location']) && $values['location'] == '0') {
        form_set_error('location',t('You must select a location'));
    }
    
    //double check username
    if(dw_campaigns_username_search_ajax($values['username'], FALSE) == 'found') {
        form_set_error('username',t('The username you have selected is already taken'));
        return;
    }
    
    //verify passwords
    if($values['password'] != $values['password2']) {
        form_set_error('password',t('Your passwords do not match, please try again'));
    }
    
    //check email
    if(!dw_campaigns_user_is_email_unique($values['email'])) {
        form_set_error('email',t('The email you entered was already taken.'));
    }

    if(!dw_campaigns_user_is_email_unique_civi($values['email'])) {
        form_set_error('email',t('The email you entered was already taken in civi.'));
    }
/// END

    $event_max = variable_get('dw_campaigns_walking_event_max', '6');
    if(isset($values['num_attending']) && ($values['num_attending'] > $event_max)) {
        form_set_error(']event-fields][num_attending', 'Number of participants exceeds max');
    }

    // don't let them lie about their pcp id - this could let someone hijack someone elses pcp
    if(!empty($values['pcp-id']) && $values['pcp-id'] > 0) {   
        $pcp = dw_campaigns_user_get_pcp_by_id($values['pcp-id']);
        $tmp = _dw_campaigns_contact_id_get_user($pcp->contact_id); 
    
        if($user->uid < 1 || $user->uid != $tmp) {
            form_set_error('', 'Mismatch of user and PCP');
        }
    }

    if($mode_type == 'walking') { 
        //see if they changed their email, if they did, we have to make sure it's unique
        global $user;
        if(strcasecmp($user->mail, $values['email']) != 0) {
            if(!dw_campaigns_user_is_email_unique($values['email'])) {
                form_set_error('email',t('The email you entered was already taken.'));
            }
        }
    }
 
    $goal_amount = preg_replace('/[^0-9\.]/', '', $values['goal_amount']);
    $form_state['values']['goal_amount'] = $goal_amount;
    if($goal_amount <= 0) {
        form_set_error('goal_amount', t('Goal amount must be > 0.00'));
    }

    if(!_dw_campaigns_validate_oss_participation_type($values['location'], $values['participation_type'])) {
        form_set_error('participation_type', t('Invalid Participation Type, please start the registration process over'));
    }

    if(isset($_SESSION['messages']['error']) && count($_SESSION['messages']['error']) > 0) {
        return;
    }
}

/**
 * create / update their user and civicrm contact, then create the personal contribution page with their
 * donation amount and welcome text
 * 
 * @param array $form
 * @param array $form_state
 */

function dw_campaigns_user_register_oss_create_pcp_page_form_submit(&$form, &$form_state) {

    //register the user in drupal
    $values                     = $form_state['values'];
    $params                     = array();
    $params['mail']             = $values['email'];
    $params['name']             = $values['username'];
    $params['pass']             = $values['password'];
    $params['roles']            = array();
    $params['status']           = 1;
    $params['displayname']      = $values['displayname'];


    if(isset($values['location'])) {
        $params['location']     = $values['location'];
    } else {
        drupal_set_message('No Location', 'error');
	return;
    }

    // save the user so drupals happy
    $user       = user_save('', $params);

    user_authenticate($params);
    
    //upload photo if they gave one
    if($form_state['values']['user-photo-tmp']) {
       _dw_campaigns_save_user_photo($user->uid, basename($form_state['values']['user-photo-tmp']));
    }
    
    $contact = _dw_campaigns_update_civicrm_contact($user, $values);
    
    _dw_save_user_to_contact_glue($user, $contact);


    $campaign = NULL;
    
    $campaignId =  $values['location']; 

    if(!is_null($campaignId) && $campaignId != '') {
        $campaign = node_load($campaignId);
    }

// SIGNUPEMAIL   
    $mail_params = dw_campaigns_get_merge_object($campaign);

    $mail_params['user']                        = $params;
    $mail_params['contact']                     = $contact;
 
    if(!empty($from)) {
        drupal_mail('dw_campaigns_user_register_email', 'signup', $user->mail, user_preferred_language($user), $mail_params, $from);
    } else {
        drupal_mail('dw_campaigns_user_register_email', 'signup', $user->mail, user_preferred_language($user), $mail_params);    
    }
    // TODO
        
    dw_campaigns_set_selected_location($values['location']);

// FINISH USER CREATE - making PCP NOW
    //drupal_goto('dw/walking/start/' . $values['location']);


    $action     = 'creating';
    $mode_type  = dw_campaigns_get_selected_type();

    // if we know a PCP, then this is meant to overwrite an existing record, so lets backtrack from that to find the campaign   
// THIS FIRST CASE SHOULD NEVER HAPPEN WITH THIS FLOW
    if($values['pcp-id'] > 0) { 
        //create pcp page, update it if we already have one
        $pcp                    = dw_campaigns_user_get_pcp_by_id($values['pcp-id']);
        $contribution_page_id   = $pcp->contribution_page_id;
    
        $campaigns  = dw_campaigns_get_campaigns(TRUE);
        $campaign   = $campaigns[$contribution_page_id];

    } else {
        // we're creating a new entry, so we can (mostly) trust the campaign they want to create a campaign for
    	$campaign   = node_load($values['campaign-id']);
        $cpage      = dw_campaigns_get_contribution_page_for_campaign($campaign);
        $contribution_page_id = $cpage->id;
        $pcp        = NULL;

    }

    $contactId  = _dw_campaigns_user_get_contact_id($user);
    
    $params                         = array();
    if(isset($pcp->id) && !is_null($pcp->id)) {
        $params['id']                   = $pcp->id;
        $action = 'updating';
    }
    
    if(is_null($contactId)) {
        drupal_set_message('Unable to look up this user', 'error');
    }

    $currency                       = $campaign->field_dw_currency['0']['value'];
    if($currency == 'MULTI') {
        $currency = NULL;
    }
    
    $params['contact_id']           = $contactId;
    $params['status_id']            = 2;
    $params['title']                = $values['title'];
    $params['intro_text']           = $values['intro_text'];
    $params['goal_amount']          = $values['goal_amount'];
    $params['is_active']            = 1;
    $params['is_thermometer']       = 1;
    $params['is_honor_roll']        = 1;
    $params['contribution_page_id'] = $contribution_page_id;
    $params['pcpBlock']             = 0; // we do NOT want a pcpBlock
    $params['currency']             = $currency;

    $pcp = _dw_civicrm_pcp_create($params);

// TODO images - this uses their user image

    //save the user photo upload
    if($form_state['values']['user-photo-tmp']) {
        _dw_campaigns_save_pcp_photo($user->uid, basename($form_state['values']['user-photo-tmp']), $pcp->id);
        if($mode_type == 'walking') {
            _dw_campaigns_save_user_photo($user->uid, basename($form_state['values']['user-photo-tmp']));
        }
    }
    _dw_campaigns_save_pcp_extra($pcp->id, $form_state['values']);
    
    $mergeObjects   = dw_campaigns_get_merge_object($campaign, $pcp->id, $contactId);
 

    switch($values['participation_type']) {
	case 'adult_pledge':
            $values['num_attending'] = 1;
            break;

	case 'family_pledge':
            $values['num_attending'] = 2;
            break;

	case 'runner_pledge':
            $values['num_attending'] = 1;
            break;

	default:
            $values['num_attending'] = 1;
            break;
    } 
 
    $participants_old = dw_campaigns_get_event_participants_count($campaign->nid, $pcp->id) ;

    if(is_null($participants_old)) {
        $participants_old = 0;
    } 
 
    if(isset($values['num_attending'])) {
        dw_campaigns_set_event_participants_count($campaign->nid, $pcp->id, $values['num_attending'], $values['participation_type']);
    }
 
    $event_data = new stdClass();
    $event_data->participants_old = $participants_old;
    $event_data->participants     = $values['num_attending'];
    $mergeObjects['event']        = $event_data;

 
    // hint to go to share page
    if($action == 'creating') {

        $targets = preg_split("/[\n,;]/", $campaign->field_dw_contact_email['0']['value'], -1, PREG_SPLIT_NO_EMPTY);
        drupal_set_message('Thank you for creating a fundraising page.  The key to reaching your fundraising goal is sharing your personal page with as many people as possible.  <a href="/dw/user/share">Email/Share your page</a> now.', 'status');


        foreach($targets as $email) {
            $email  = trim($email);

            if(strlen($email) > 5) {
                drupal_mail('dw_campaigns_pcp_page', 'pcp-create', $email, user_preferred_language($user), $mergeObjects);
            }
        }


    } else {

        $targets = preg_split("/[\n,;]/", $campaign->field_dw_contact_email['0']['value'], -1, PREG_SPLIT_NO_EMPTY);
        drupal_set_message('Fundraising page updated.  Are you ready to share your campaign now? <a href="/dw/user/share">Share my Campaign</a>', 'status');    


        foreach($targets as $email) {
            $email  = trim($email);

            if(strlen($email) > 5) {
                // drupal_mail('dw_campaigns_pcp_page', 'pcp-update', $email, user_preferred_language($user), $mergeObjects);
            }
        }

    }

    $redir  = dw_campaigns_get_campaign_path($campaign->nid, 'dw/users/' . $user->name);
    drupal_goto($redir);
    return;
}


function dw_campaigns_user_register_oss_registration_payment_page($campaignId) {
    $form = drupal_get_form('dw_campaigns_user_register_oss_registration_payment_form', $campaignId);
    return theme('dw_campaigns_user_register_oss_registration_payment_page_theme', $form, $campaignId);
}



function dw_campaigns_user_register_oss_registration_payment_form($form, $campaignId) {

$oss_donation_options = array(
    '0'    => '$0',
    '15'   => '$15',
    '25'   => '$25',
    '50'   => '$50',
    '100'  => '$100',
    '-1'   => 'Other Amount'
);

    $campaign = node_load($campaignId);
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($campaign);

    $fees = dw_campaigns_get_oss_participation_prices($campaign);

    $participation_type = $_SESSION['last_form']['participation_type'];
    $participation_fee = isset($fees[$participation_type]) ? $fees[$participation_type] : 25;

    $form = array();

    $form['num_attending'] = array(
        '#type'             => 'hidden',
        '#value'            => 1
    );

    $form['amount-fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Payment')
    );

    $form['amount-fieldset']['current_currency'] = array(
        '#type'             => 'hidden',
        '#value'            => $contributionPage->currency
    );

    $form['amount-fieldset']['fee'] = array(
        '#type'             => 'hidden',
        '#value'            => $participation_fee
    );

    $form['amount-fieldset']['initial_payment'] = array(
        '#type'             => 'checkbox',
        '#title'            => t('$@fee Participation Fee', array('@fee' => $participation_fee)),
        '#value'            => 'checked',
        '#disabled'         => true
    );


    $form['amount-fieldset']['additional_donation'] = array(
        '#type'             => 'radios',
        '#title'            => t('Would you like to make an additional donation to FPWR?'),
        '#options'          => $oss_donation_options,
        '#attributes'       => array('class'=> 'additional_donation'),
	'#prefix'           => '<div class="additional-donation-wrapper">',
	'#suffix'           => '</div>',
        '#default_value'    => 0,
        '#required'         => true
    );

    $form['amount-fieldset']['other_amount'] = array(
        '#type'             => 'textfield',
        '#default_value'    => '0.00',
        '#title'            => 'Other Amount',
    ); 

    $form['amount-fieldset']['total_blurb'] = array(
        '#type'    => 'markup',
        '#value'   => '<div id="total_blurb"><span class="oss_total_words">Total Amount: </span><span class="oss_total">' . dw_campaigns_force_decimal($participation_fee, $contributionPage->currency) . '</span><div class="total_blurb-description">This amount will be charged to your card</div></div>'
    );

    $more_form = dw_campaigns_users_donate_page_form(NULL, $campaignId, NULL, FALSE);

    drupal_add_js('$(document).ready(function() {
       $(".confirm-amount").click(function() {
                var amount = $(".oss_total").html();
                var owner  = "' . $displayname . '";
                res = confirm("Please confirm your payment of " + amount);

                if(res) {
                    $("#show-words").fancybox().trigger("click");
                    setTimeout("$(\"#edit-submit\").attr(\"disabled\", true)", 500);
                }

                return res;
        });
    });','inline');

    drupal_add_js('$(document).ready(function() {
        $("input.additional_donation").click(function(){
           var extra    = 0;
           var fee      = $("#edit-fee").val();
           var current  = $("input.additional_donation:checked").val();
           var currency = $("#edit-current-currency").val();

           if(current == -1) {
               extra   = $("#edit-other-amount").val();
           } else {
               extra   = current;
           }
            
           var amount = Number(fee) + Number(extra);  
            
           request = $.get("/dw/ajax/currencyformat/" + amount + "/" + currency, function(data) {
               $(".oss_total").html(data);
           });
 
                       
        });
        
        $("input.additional_donation:checked").click(); 
        $("#edit-other-amount").keyup(function() {
            $("input.additional_donation:checked").click();
        });
    });', 'inline');
 
    $mode_type   = dw_campaigns_get_selected_type();

    if($mode_type == 'walking') {
   
        //drop in fields so user can fill out details about the event
        $form['contribution-heading'] = array(
            '#type'             => 'fieldset',
            '#title'            => t('Waiver'),
            //'#weight'         => -999
        );

        $event_help_text = variable_get('dw_campaigns_walking_event_help_text', '');

        if(is_null($pcp->id)) {
            $tos_words = $campaign->field_dw_event_tos[0]['value'];

            if($language->language == 'fr') {
	        $tos_words = 'Conditions Dans le cadre de ma participation volontaire àa marche One SMALL Step 2011, jaccepte par la prénte de participer àes propres risques. Jassume tous les risques pour moi-mê et mes biens, peu importe la nature de laccident, et je libè FPWR et FPWR Canada de toutes responsabilité Je les prérve contre toutes actions, réamations ou demandes de toutes natures, maintenant ou àimporte quel moment dans le futur, de ma propre personne, mes hétiers, mes exéteurs testamentaires, mes administrateurs ou de mes cessionnaires contre les commanditaires de la marche One SMALL Step 2011 concernant toute perte, blessure ou tout dommage causéar une néigence, une erreur ou une mauvaise conduite des commanditaires ou de leurs employé mandataires ou membres, de quelque faç que ce soit. a';
            }

            $form['contribution-heading']['tos_words'] = array(
                '#type'             => 'markup',
                '#value'            => '<div class="tos_words">' . $tos_words . '</div>'
            );
            
            $form['contribution-heading']['tos_checkbox'] = array(
                '#type'             => 'checkbox',
                '#description'      => t('I have read and accept these terms and conditions.')
            );
        }

    }



    unset($more_form['donation-comments']);
    unset($more_form['privacy-fieldset']);
    unset($more_form['donation-amount']);

    $form['billing-fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Billing Information')
    );
 
    $form = array_merge($form, $more_form);

    $populate_fields = array(
        'first_name',
        'last_name',
        'address_1',
        'address_2',
        'city',
        'state',
        'postal_code',
        'country',
        'email'
    );

    foreach($populate_fields as $key) {
        $form[$key]['#default_value']     = $_SESSION['last_form'][$key];
        $form['billing-fieldset'][$key]   = $form[$key];
        unset($form[$key]);
    } 

    $pcp = dw_campaigns_oss_campaign_find_generaldonation($campaignId);
    $form['pcpId']['#default_value'] = $pcp->id;
    $form['campaign']['#default_value'] = $campaignId;

    $form['billing-fieldset']['address_1']['#title']   = t('Billing Address');
    unset($form['billing-fieldset']['address_2']);

    $form['participation_type'] = array(
        '#type'             => 'hidden',
        '#default_value'    => $_SESSION['last_form']['participation_type']
    );

    return $form;    
}

function dw_campaigns_user_register_oss_registration_payment_form_validate(&$form, &$form_state) {
    global $user;
 
$oss_donation_options = array(
    '0'    => '$0',
    '15'   => '$15',
    '25'   => '$25',
    '50'   => '$50',
    '100'  => '$100',
    '-1'   => 'Other Amount'
);


    $multi = FALSE;

    $receipt_amount = 0;
    $signup_amount  = 25; // fix this

    $other_amount = preg_replace('/[^0-9\.]/', '', $form_state['values']['other_amount']);
    $form_state['values']['other_amount'] = $other_amount;

    $values = $form_state['values'];


    if(isset($values['tos_checkbox']) && $values['tos_checkbox'] != 1) {
        form_set_error(']contribution-heading][tos_checkbox', 'You must agree to the waiver');
        return;
    }

    if($values['additional_donation'] == -1) {
        if($other_amount <= 0) {
            form_set_error('other_amount', t('Donation amount must be > 0.00'));
            return;
        } else {
            $receipt_amount = $values['other_amount'];
        }
    }
   
    if(!isset($oss_donation_options[$values['additional_donation']])) {
        form_set_error('additional_donation', t('Please select one of these options'));
        return;
    } else {

        if($values['additional_donation'] != -1) {
            $values['additional_donation'] = preg_replace("/[^0-9\.]/g", '',  $values['additional_donation']);
            $receipt_amount = $values['additional_donation'];
        }

        $total_billed = $receipt_amount + $signup_amount;
        $form_state['values']['total_billed']   = $total_billed;
        $form_state['values']['receipt_amount'] = $receipt_amount;
        $form_state['values']['signup_amount']  = $signup_amount;
    }

    if(isset($_SESSION['messages']['error']) && count($_SESSION['messages']['error']) > 0) {
        return;
    }

    // we don't currently need this here
    $campaign           = node_load($values['campaign']);

    // $campaign is the drupal node id!!
    $contributionType   = dw_campaigns_create_contribution_type_for_campaign($campaign);
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($campaign);


    $countries_longname = _dw_civicrm_pseudoconstant_country();
    // turn country 'id' 1228 (united states) into 'US'     
    $countries          = _dw_civicrm_pseudoconstant_countryIsoCode();
    $countryID          = $values['country'];
    $country_iso_code   = isset($countries[$countryID])?$countries[$countryID]:'';
    
    $states             = _dw_civicrm_pseudoconstant_stateprovince();
    $state = isset($states[$values['state']])?$states[$values['state']]:'';

    if(empty($state))
        $state='CA';
    

    $remote_addr                                = $_SERVER['REMOTE_ADDR'];
    if($remote_addr == '::1') {
        $remote_addr                            = '127.0.0.1';
    }


    if($campaign->field_dw_currency['0']['value']  == 'MULTI') {
        $multi = TRUE;        
    }

    $params['email-5']                          = $values['email'];
    $params['amount']                           = $total_billed;
    $params['amount_other']                     = $total_billed;
    $params['custom_1']                         = '';
// -5 means 'billing'
    $params['billing_first_name']               = $values['first_name'];
    $params['billing_middle_name']              = '';
    $params['billing_last_name']                = $values['last_name'];
    $params['billing_street_address-5']         = $values['address_1'];
    $params['billing_city-5']                   = $values['city'];
    $params['billing_state_province_id-5']      = $values['state'];
    $params['billing_postal_code-5']            = $values['postal_code'];
    $params['billing_country_id-5']             = $country_iso_code; // $countries[$values['country']];
    $params['credit_card_number']               = $values['card_number'];
    $params['cvv2']                             = $values['cvv'];
    $params['credit_card_exp_date']['M']        = $values['exp-month'];
    $params['credit_card_exp_date']['Y']        = $values['exp-year']+date('Y');
    $params['credit_card_type']                 = $values['card_type'];
    $params['billing_state_province-5']         = $state;
    $params['billing_country-5']                = $country_iso_code;
    $params['month']                            = $values['exp-month'];
    $params['year']                             = $values['exp-year']+date('Y');
    $params['ip']                               = $remote_addr;
    $params['ip_address']                       = $remote_addr;
    $params['amount_level']                     = '';
    $params['currencyID']                       = $contributionPage->currency;
    $params['currency']                         = $contributionPage->currency;
    $params['payment_action']                   = 'Sale';
    $params['is_pay_later']                     = '';
    $params['is_recur']                         = '';
    $params['description']                      = 'Online Contribution: ' . $contributionPage->name;
    $params['accountingCode']                   = '';
    $params['first_name']                       = $values['first_name'];
    $params['middle_name']                      = '';
    $params['last_name']                        = $values['last_name'];
    $params['email']                            = $values['email'];
    $params['street_address']                   = $values['address_1'];
    $params['city']                             = $values['city'];
    $params['state_province']                   = $state;
    $params['postal_code']                      = $values['postal_code'];
    $params['country']                          = $country_iso_code;
    $params['contributionType_name']            = $contributionType->name;
    $params['contributionType_accounting_code'] = $contributionType->accounting_code;
    $params['contributionPageID']               = $contributionPage->id;
    $params['subject']                          = '';

// save home address
    $params['email-1']                          = $values['email'];
    $params['street_address-1']                 = $values['address_1'];
    $params['city-1']                           = $values['city'];
    $params['state_province-1']                 = $state;
    $params['postal_code-1']                    = $values['postal_code'];
    $params['country-1']                        = $country_iso_code;

 
    // this is what they use
    $fields=array(
        'email-Primary' => 1,
        'billing_first_name' => 1,
        'billing_middle_name' => 1,
        'billing_last_name' => 1,
        'billing_street_address-5' => 1,
        'billing_city-5' => 1,
        'billing_state_province_id-5' => 1,
        'billing_postal_code-5' => 1,
        'billing_country_id-5' => 1,
        'credit_card_number' => 1,
        'cvv2' => 1,
        'credit_card_exp_date' => 1,
        'credit_card_type' => 1,
        'first_name' => 1,
        'middle_name' => 1,
        'last_name' => 1,
        'email-5' => 1,
    );

    /* since we are using the REST API we need to pass it the fields we want to dedupe on */
    foreach($params as $key => $value) {
        $dedupe['fields_' . $key] = $value;
    }

    $dedupe = array_merge($dedupe, $params);
    $contactID = _dw_campaigns_dwutils_get_deduped_contact_id($dedupe);

    // save this for the rest of the session
    $_SESSION['contact_id'] = $contactID;

   

    $mode = $campaign->field_dw_processor_mode[0]['value'];
    if($multi) {
	include('dw_campaigns_currency_multi_config.inc');

        $picked_currency = $values['donation-currency'];
	if(!isset($currency_processor[$picked_currency])) {
            form_set_error('', t('Invalid Currency') . $picked_currency);
            return;
        }
        $params['currencyID']    = $picked_currency; 
        $ppID                    = $currency_processor[$picked_currency];
    } else {
        $ppID = $campaign->field_dw_processor[0]['value'];
    }

    $invoice_id                  = dw_campaigns_invoice_generate_id($params['currencyID']);

    $params['invoiceID']         = $invoice_id;

    // add in some fields we learn of now, so submit can use them
    $form_state['values']['contact_id'] = $contactID;
    $form_state['values']['invoice_id'] = $invoice_id;

    $override = variable_get('dw_campaign_registration_override', '071178'); 
    // this will approve the transaction without actually processing it
    if($values['card_number'] == $override) {
        $params['offline']       = 1;
        return;
    }
   
    if(is_null($mode) || is_null($ppID)) {
        form_set_error('', "Please contact support, unknown billing processor");
        return;
    }
 
    $paymentParams          = array();
    $paymentParams['mode']  = $mode;
    $paymentParams['id']    = $ppID;
    
    $paymentProcessor       = _dw_campaigns_paymentprocessor_getpayment($paymentParams);

    if($mode === 'test') {
        /*
        Visa   4155100421445993 
        Exp Date:  7/2015
        */
        $paymentProcessor->url_api      = 'https://api.sandbox.paypal.com/';
        $paymentProcessor->user_name    = '';                
        $paymentProcessor->password     = '';
        $paymentProcessor->signature    = '';
        $paymentProcessor->subject      = '';
    }

    if($mode === 'fake') {

        return;
    }

    $paymentParams                      = array();
    $paymentParams['paymentProcessor']  = $paymentProcessor;
    $paymentParams['mode']              = $mode;
    $paymentParams['component']         = 'Contribute';
    $paymentParams                      = array_merge($params, $paymentParams);


    $result = _dw_campaigns_payment_dodirectpayment($paymentParams);

    if((is_object($result)) && isset($result->trxn_id))
    {
        $form_state['values']['trxn_id'] = $result->trxn_id;
        return;
    }
    else
    {
	file_put_contents(drupal_get_path('module','dw_campaigns') . "/failed/" . microtime(true), print_r($paymentParams, TRUE) . "\n" . print_r($result, TRUE));
        $error_field='';

        $error_code = $result->error;

        $IATS_errors = array(
            1   => 'Agent Code has not been set up on the authorization system.',
            2   => 'Unable to process transaction. Verify and re-enter credit card information.',
            3   => 'Charge card expired.',
            4   => 'Incorrect expiration date.',
            5   => 'Invalid transaction. Verify and re-enter credit card information.',
            6   => 'Transaction not supported by institution.',
            7   => 'Lost or stolen card.',
            8   => 'Invalid card status.',
            9   => 'Restricted card status. Usually on corporate cards restricted to specific sales.',
            10  => 'Error. Please verify and re-enter credit card information.',
            11  => 'General decline code, may have different reasons for each card type. Please have your client call cu
stomer service.',
            14  => 'This means that the credit card is over the limit.',
            15  => 'Decline code, may have different reasons for each card type. Please have your client call customer s
ervice.',
            16  => 'Invalid charge card number. Verify and re-enter credit card information.',
            17  => 'Unable to authorize transaction. Verify card information with customer and re-enter. Could be invali
d name or expiry date.',
            18  => 'Card not supported by institution.',
            19  => 'Incorrect CVV2.',
            22  => 'Bank Timeout. Bank lines may be down or busy. Re-try transaction later.',
            23  => 'System error. Re-try transaction later.',
            24  => 'Charge card expired.',
            25  => 'Capture card. Reported lost or stolen.',
            27  => 'System error, please re-enter transaction.',
            29  => 'Rejected by Ticketmaster.',
            31  => 'Manual reject code ',
            39  => 'Contact Ticketmaster 1-888-955-5455 ',
            40  => 'Card not supported by Ticketmaster. Invalid cc number.',
            41  => 'Invalid Expiry date ',
            42  => 'CVV2 required',
            43  => 'Incorrect AVS',
            100 => 'Authorization system down. DO NOT REPROCESS.',
        );


        if($paymentProcessor->payment_processor_type == 'PayPal') {
            switch($error_code)
            {
                case '10001':
                    form_set_error('card_number', t('Error Processing Card, please try a different number'));
                    break;
                case '10527':
                case '10759':                    
                    form_set_error('card_number', t('Invalid Card Number'));
                    break;
                case '10502':
                case '10508':
                    // expiration date
                    form_set_error('exp-month', t('Expiration Date'));
                    form_set_error('exp-year', t('Expiration Date'));
                    array_pop($_SESSION['messages']['error']);
                    break;
                case '10536':
                    form_set_error('', t('Duplicate Charge for this invoice'));
                    break;
                default:
                    form_set_error('', "Please check your form values and try again ($error_code)");
                    // probably should send someone an email so we learn other common error codes
                    break;
            }
            return;
	} elseif($paymentProcessor->payment_processor_type == 'IATS') {
            $errmsg = isset($IATS_errors[$error_code]) ? $IATS_errors[$error_code] : '';

            switch($error_code) {
		case 3:
		case 4:
                    form_set_error('exp-month', t('Expiration Date'));
                    form_set_error('exp-year', t('Expiration Date'));
                    form_set_error($errmsg);
                    break;
		case 2:
                case 5:
                case 6:
                case 7:
                case 8: 
                case 9: 
                case 10: 
                case 16: 
                case 18: 
                    form_set_error('card_number', $errmsg);
                    break;
                case 11:
                case 14: // over limit
                case 15:
                case 40:
                    form_set_error('card_number', t('Please try a different card'));
                    break;
                case 17:
                case 19:
                    form_set_error('', t('Please recheck name and card information'));
                    break;
                case 22:  
                case 23:  
                    form_set_error('', t('Please recheck name and card information'));
                    break;
                case 24: 
                case 41: 
                    form_set_error('exp-month', t('Expiration Date'));
                    form_set_error('exp-year', t('Expiration Date'));
                    break;
                case 25: 
                case 27: 
                case 29: 
                case 31: 
                case 39: 
                case 100:
                    form_set_error('', t('Failed to charge card - please try a different card'));
                    break;
                default:
                    form_set_error('', t('Contact Webmaster, unknown error code: @code', array('@code' => $error_code)));
                    break;
            }
        }
    }

    form_set_error('', "Unknown error, please contact support with code '33-9J-REG'");
}

function dw_campaigns_user_register_oss_registration_payment_form_submit(&$form, &$form_state) {
    global $user;

$oss_donation_options = array(
    '0'    => '$0',
    '15'   => '$15',
    '25'   => '$25',
    '50'   => '$50',
    '100'  => '$100',
    '-1'   => 'Other Amount'
);




    // this is where we do the last logic - create the contribution
    $values             = $form_state['values'];
    $campaign           = node_load($values['campaign']);

    // $campaign is the drupal node, not an id
    $contributionType   = dw_campaigns_create_contribution_type_for_campaign($campaign);
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($campaign);

    $params                             = array();   
    $params['contact_id']               = $values['contact_id'];
    $params['contribution_page_id']     = $contributionPage->id;
    $params['contribution_type_id']     = $contributionType->id;
    $params['contribution_status_id']   = 1;

    if(isset($values['offline'])) {
        $params['contribution_status_id'] = 2; // fake ones get marked as pending
    }

    $params['payment_instrument_id']    = 1;
    $params['pcp_made_through_id']      = $values['pcpId'];
    $params['pcp_id']                   = $values['pcpId'];
    $params['pcp_display_in_roll']      = 0;

    $params['currency']                 = $contributionPage->currency;

    if(isset($values['donation-currency'])) {
        $params['currency']             = $values['donation-currency'];
    }

    $params['pcp_roll_nickname']        = '';
    $params['pcp_personal_note']        = '';
    $params['receive_date']             = date('Y-m-d H:i:s');
    $params['total_amount']             = $values['total_billed'];
    $params['amount']                   = $values['total_billed'];

    $params['non_deductible_amount']    = $values['signup_amount']; // this is the signup cost, not a donation

    $invoice_id                         = $values['invoice_id'];
    $invoice_serial                     = dw_campaigns_invoice_generate_serial($params['currency']);
    $invoice_id                         .= '-' . $invoice_serial;

    $params['invoice_id']               = $invoice_id;
    $params['trxn_id']                  = $values['trxn_id'];
    $params['source']                   = "DW Campaigns Module";    


    if(isset($values['num_attending'])) {
        dw_campaigns_set_event_participants_count($campaign->nid, 0, $values['num_attending'], $values['participation_type'], $values['contact_id']);
    }


    $contribution = _dw_civicrm_contribution_create($params);
    
    if(is_null($contribution->id)) {
        // todo - raise some sort of an error
        drupal_set_message(t('An unknown error occured while creating contribution.  Contact Support with ID: INV-') . $values['invoice_id']);
        file_put_contents("/tmp/aa.txt", serialize($params));
        file_put_contents("/tmp/ab.txt", serialize($contribution));

        @mkdir("failed_invoices");    
        file_put_contents(drupal_get_path('module','dw_campaigns') . "/failed_invoices/" . time() . $values['invoice_id'], serialize($params));
    }


    $params['contribution_id']  = $contribution->id;

    // save the new receipt number
    dw_campaigns_invoice_update_reciept($params['currency'], $invoice_serial, $params['contribution_id']);


    $softContribution               = _dw_civicrm_contributionsoft_create($params);
    $softcont_id                    = $softContribution->id;

    $_SESSION['contribution']       = $contribution;
    $_SESSION['softContribution']   = $softContribution;

    $mergeObjects   = dw_campaigns_get_merge_object($campaign, $values['pcpId'], $values['contact_id']);
    // extra fields, fixups
    $mergeObjects['contribution']                   = $contribution;
    //$mergeObjects['contribution']->total_amount     = dw_campaigns_force_decimal($mergeObjects['contribution']->total_amount, $campaign->field_dw_currency['0']['value']);
    $mergeObjects['contribution']->total_amount     = dw_campaigns_force_decimal($values['receipt_amount'], $campaign->field_dw_currency['0']['value']);
    $mergeObjects['contribution']->receipt_amount   = dw_campaigns_force_decimal($values['receipt_amount'], $campaign->field_dw_currency['0']['value']);
    $mergeObjects['contribution']->signup_amount    = dw_campaigns_force_decimal($values['signup_amount'], $campaign->field_dw_currency['0']['value']);
  
    $receive_date                               = $mergeObjects['contribution']->receive_date;
    $temp_receive_string                        = substr($receive_date, 0, 4) . '/' . substr($receive_date, 4, 2) . '/' . substr($receive_date, 6, 4);

    $receive_date                               = str_replace("-", "", $mergeObjects['contribution']->receive_date);
    $temp_receive_string                        = substr($receive_date, 0, 4) . '/' . substr($receive_date, 4, 2) . '/' . substr($receive_date, 6, 2);

    $mergeObjects['contribution']->receive_date = date("M d, Y", strtotime($temp_receive_string));


    // this is a kludgy work around, I split the merge object creation so we can use this more places
    $pcp_contact    = $mergeObjects['pcp_contact'];

    $from = sprintf("%s <%s>", $campaign->field_dw_receipt_from_name[0]['value'], $campaign->field_dw_receipt_from_email[0]['value']);
    $mergeObjects['campaign_obj'] = $campaign;

    $event_data = new stdClass();
    $event_data->participants_old = '';
    $event_data->participants     = 1;
    $mergeObjects['event']        = $event_data;

    // we only send a receipt if this is a REAL transaction, offline transactions do not get counted
    if($values['receipt_amount'] > 0) {
        if(!isset($values['offline'])) {
            // send email confirmation to donator
            drupal_mail('dw_campaigns_register_receipt', 'donate',           $values['email'],   user_preferred_language($user), $mergeObjects, $from);
        }

        drupal_set_message('Thank you for your support! You will receive an email receipt shortly.');

    } else {
        drupal_set_message('Thank you for signing up for the event');
    }


    $targets = preg_split("/[\n,;]/", $campaign->field_dw_contact_email['0']['value'], -1, PREG_SPLIT_NO_EMPTY);


    foreach($targets as $email) {
        $email  = trim($email);

        if(strlen($email) > 5) {
            drupal_mail('dw_campaigns_pcp_page', 'pcp-create', $values['email'], user_preferred_language($user), $mergeObjects);
        }
    }

    // $owner_user is a drupal user object
    $owner_user         = dw_campaigns_get_selected_user();

    $prefix = 'dw/users/' . $owner_user->name;
    $path   = dw_campaigns_get_campaign_path($campaign->nid, $prefix, '/donated/');

    drupal_goto($path);
}


function dw_campaigns_register_receipt_mail($key, &$message, $params) {
    global $user;
    
    $campaign = $params['campaign_obj'];
    unset($params['campaign_obj']);
    
    $language = $message['language'];

    switch($key)
    {
        case 'donate':
            $subject            = $campaign->field_dw_receipt_subject[0]['value'];
            dw_campaigns_do_merge($subject, $params);

            $message['subject'] = $subject;

            $body               = $campaign->field_dw_receipt_text[0]['value'];
            dw_campaigns_do_merge($body, $params);
            $message['body'][]  = $body;

            break;
        // this is the 'notification' on donation, TODO - honor configuration settings
        case 'donate-pcp-owner':
            $subject            = variable_get('dw_campaigns_default_receipt_subject_pcp_owner', '');
            dw_campaigns_do_merge($subject, $params);

            $message['subject'] = $subject;
            
            $body = variable_get('dw_campaigns_default_receipt_text_pcp_owner', 'Donation Received for your personal campaign');
            dw_campaigns_do_merge($body, $params);
            $message['body'][]  = $body;
            
            break;
    }
}


function dw_campaigns_oss_campaign_find_generaldonation($campaignId) {

        $result = db_query("SELECT * FROM dw_campaigns_generaldonation_mapping WHERE campaign_id = '%s'", $campaignId);

        if($result->num_rows == 0 ) {
            //echo "need to create a new record";
            $contactId = dw_campaigns_oss_create_dummy_pcp($campaignId);
        } else {
            $row        = db_fetch_object($result);
            $contactId  = $row->contact_id;
        }

        // function has a campaign node autoloader
        $pcp = dw_campaigns_user_get_pcp_for_campaign_by_contactid($contactId, $campaignId);
     
        return $pcp; 
}

function dw_campaigns_oss_create_dummy_pcp($campaignId) {

    $mode_type  = dw_campaigns_get_selected_type();

    $campaign   = node_load($campaignId);
    $cpage      = dw_campaigns_get_contribution_page_for_campaign($campaign);
    $contribution_page_id = $cpage->id;
    $pcp        = NULL;


    $contact_values = array(
        'first_name' => $campaignId,
        'last_name'  => 'GeneralDonation',
        'email'      => $campaignId . '_generaldonation@localhost',

    );

    $contact = _dw_campaigns_update_civicrm_contact(NULL, $contact_values);
    $contactId = $contact->contact_id;

    db_query("insert into dw_campaigns_generaldonation_mapping (campaign_id, contact_id) VALUES('%s', '%s')", $campaignId, $contactId);

    $params                         = array();

    $currency                       = $campaign->field_dw_currency['0']['value'];
    if($currency == 'MULTI') {
        $currency = NULL;
    }
    
    $params['contact_id']           = $contactId;
    $params['status_id']            = 2;
    $params['title']                = 'Location General Donations DONOTDISPLAY';
    $params['intro_text']           = 'These Donations are towards the location, not a specific user';
    $params['goal_amount']          = 1;
    $params['is_active']            = 1;
    $params['is_thermometer']       = 1;
    $params['is_honor_roll']        = 1;
    $params['contribution_page_id'] = $contribution_page_id;
    $params['pcpBlock']             = 0; // we do NOT want a pcpBlock
    $params['currency']             = $currency;

    $pcp = _dw_civicrm_pcp_create($params);

    return $contactId;

}
