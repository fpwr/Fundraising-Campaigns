<?php
/*
    'restUrl'	=> 'http://172.16.0.175/sites/all/modules/civicrm/extern/rest.php?q=civicrm',
    'siteKey'	=> '0392c182c832f798500cca3d8d983b56',
    'loginName'	=> 'admin',
    'loginPass'	=> '!fpwr2003!',
*/

function dw_campaigns_civicrm_config() {
    
    $form = array();
    
    $form['civi_settings']['dw_campaigns_cfg_rest_url'] =  array(
        '#title'            => 'Civicrm REST Interface URL',
        '#type'             => 'textfield',
        '#description'      => t('Civi REST Interface URL ( http://samplecivi.com/sites/all/modules/civicrm/extern/rest.php?q=civicrm )'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_rest_url', '')
    );
    
    $form['civi_settings']['dw_campaigns_cfg_sitekey'] = array(
        '#title'            => 'REST Interface Site Key',
        '#type'             => 'textfield',
        '#description'      => t('Civi REST Interface Site Key (CIVICRM_SITE_KEY from civicrm.settings.php'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_sitekey', '')           
    );
    
    $form['civi_settings']['dw_campaigns_cfg_loginname'] = array(
        '#title'            => 'REST Interface Login Name',
        '#type'             => 'textfield',
        '#description'      => t('Civi Login Name'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_loginname', '')           
    );
            
    $form['civi_settings']['dw_campaigns_cfg_loginpass'] = array(
        '#title'            =>  'REST Interface Login Password',
        '#type'             => 'textfield',
        '#description'      => t('Civi Login Password'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_loginpass', '')           
    );

    $form['civi_settings']['dw_campaigns_cfg_base_url'] =  array(
        '#title'            => 'Civicrm Public URL',
        '#type'             => 'textfield',
        '#description'      => t('Base URL to reach backends civicrm pages ( http://samplecivi.com )'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_base_url', '')
    );
    
    return system_settings_form($form);

}

function dw_campaigns_settings() {
    $form = array();

    
    $form['mode_settings'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('DW Campaign Module Settings'),
        '#description'  => t('Configure Operating Settings')
    );
    
    $form['mode_settings']['dw_campaigns_operating_mode'] = array(
        '#type'             => 'select',
        '#options'          => array(
            'derby'         => 'Derby',
            'walking'       => 'Walking',
        ),
        '#default_value'    => variable_get('dw_campaigns_operating_mode', 'derby')
    );
    $form['mode_settings']['dw_campaigns_organization_name'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Organization Name'),
        '#description'      => t('Global token available for configuring email messages.'),
        '#default_value'    => variable_get('dw_campaigns_organization_name', 'our cause')
    );
    
    
    $form['mode_settings']['dw_campaigns_organization_name_abbreviated'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Organization Name Abbreviation'),
        '#description'      => t('Global token available for configuring email messages.'),
        '#default_value'    => variable_get('dw_campaigns_organization_name_abbreviated', '')
    );
    
    
    
    $homepage = variable_get('dw_campaigns_homepageurl', '');
    if(empty($homepage)) {
        $cur_host = $_SERVER['HTTP_HOST'];
        $homepage = 'http://' . $cur_host . '/dw/';
    }
    
    $form['mode_settings']['dw_campaigns_homepageurl'] = array(
        '#type'             => 'textfield',
        '#title'            => t('This sites homepage URL'),
        '#description'      => t('Campaign site homepage url available for configuring email messages.'),
        '#default_value'    => $homepage
    );

    
    $form['mode_settings']['dw_campaigns_organization_website'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Your organizations website URL'),
        '#description'      => t('Organization homepage url available for configuring email messages.'),
        '#default_value'    => variable_get('dw_campaigns_organization_website', '')
    );

    $form['mode_settings']['dw_campaign_max_intro_text_chars'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Maximum Number of Characters Allowed for Personal Campaign Intro'),
        '#description'      => t('User will not be able to type once this limit is reached'),
        '#default_value'    => variable_get('dw_campaign_max_intro_text_chars', 500)
    );

    $form['mode_settings']['dw_campaign_offline_transaction_override'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Numeric code for offline transactions'),
        '#description'      => t('Enter this code into the Credit Card field to credit a personal campaign without charging a card'),
        '#default_value'    => variable_get('dw_campaign_offline_transaction_override', 'NONXCZFfsAFSDA')
    );

    $form['mode_settings']['dw_campaign_default_pcp_goal'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Default Personal Campaign Goal'),
        '#description'      => t('This is the initial value for a new Personal Campaign Goal'),
        '#default_value'    => variable_get('dw_campaign_default_pcp_goal', '100.00')
    );



    
    $form['derby_specific'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('Derby Specific Settings'),
        '#description'  => t('Configure Derby Settings')
    );

    $form['derby_specific']['dw_campaigns_default_campaign'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Node id of default campaign'),
        '#default_value'    => variable_get('dw_campaigns_default_campaign', '4')
    );

    $form['derby_specific']['dw_campaigns_derby_about_derby'] = array(
        '#title'            => 'Address for "About the Derby" link',
        '#type'             => 'textfield',
        '#description'      => t('URL for About the Derby link on the Banner Menu.'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_derby_about_derby', '')           
    );


    
    $form['walking_specific'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('Walking Specific Settings'),
        '#description'  => t('Configure Walking Settings')
    );
    
    
    $form['walking_specific']['dw_campaigns_walking_distance_unit'] = array(
        '#title'            => 'Distance Measurement Unit',
        '#type'             => 'select',
        '#options'          => array(
            'M'             => 'Miles',
            'K'             => 'Kilometers',
        ),
        '#default_value'    => variable_get('dw_campaigns_walking_distance_unit', 'M')           
    );
    
    $form['walking_specific']['dw_campaigns_host_submit_email'] = array(
        '#title'            => 'Host Submission Email',
        '#type'             => 'textfield',
        '#description'      => t('This is the email that gets sent host form submissions'),
        '#default_value'    => variable_get('dw_campaigns_host_submit_email', '')
    );

    $form['walking_specific']['dw_campaigns_fundraising_goal_override'] = array(
        '#title'            => 'Fundraising Goal Override',
        '#type'             => 'textfield',
        '#description'      => t('If this value > 0 it will be used for the thermometer "goal"'),
        '#default_value'    => variable_get('dw_campaigns_fundraising_goal_override', '0')
    );
    
    $form['walking_specific']['dw_campaigns_fundraising_goal_start_value'] = array(
        '#title'            => 'Fundraising Goal Starting Amount',
        '#type'             => 'textfield',
        '#description'      => t('If this value > 0 it will be used as the starting amount towards the thermometer "goal"'),
        '#default_value'    => variable_get('dw_campaigns_fundraising_goal_start_value', '0')
    );
    
    $form['walking_specific']['dw_campaigns_walking_event_help_text'] = array(
        '#title'            => 'Event Registration Help Text',
        '#type'             => 'textarea',
        '#description'      => t('Displayed during Page creation '),
        '#default_value'    => variable_get('dw_campaigns_walking_event_help_text', '')
    );
    
    $form['walking_specific']['dw_campaigns_walking_event_max'] = array(
        '#title'            => 'Event Registration Max Participants',
        '#type'             => 'textfield',
        '#description'      => t('Maximum number of participants'),
        '#default_value'    => variable_get('dw_campaigns_walking_event_max', '6')
    );
    
    $form['walking_specific']['dw_campaigns_walking_event_tos'] = array(
        '#title'            => 'Event Registration Terms of Service Text',
        '#type'             => 'textarea',
        '#description'      => t('Displayed during Page creation '),
        '#default_value'    => variable_get('dw_campaigns_walking_event_tos', '')
    );
   
    $form['walking_specific']['dw_campaign_hoster_response_subject'] = array(
        '#title'            => 'Event Host Request Email Response Subject',
        '#type'             => 'textfield',
        '#description'      => t('Subject of auto reply email sent to Hostee (when host request is made)'),
        '#default_value'    => variable_get('dw_campaign_hoster_response_subject', 'Thank you for expressing interest in hosting an event')
    );

    $form['walking_specific']['dw_campaign_hoster_response_body'] = array(
        '#title'            => 'Event Host Request Email Response Body',
        '#type'             => 'textarea',
        '#description'      => t('Body of auto reply email sent to Hostee (when host request is made)'),

        '#default_value'    => variable_get('dw_campaign_hoster_response_body', 'You will receive follow up communication soon.')
    );


    $form['walking_specific']['dw_campaigns_pcp_create_host_notify'] = array(
        '#title'            => 'Personal Campaign Creation Host Notify Email',
        '#type'             => 'textarea',
        '#description'      => t('Body of email sent to Host when a PCP is created (# of participants)'),
        '#default_value'    => variable_get('dw_campaigns_pcp_create_host_notify', 'Number of Walk Event Participants {{event}{participants}}')
    ); 

    $form['walking_specific']['dw_campaigns_participation_type_blurb'] = array(
        '#title'            => 'Participation Type Blurb',
        '#type'             => 'textarea',
        '#description'      => t('These words will show up on the register_oss page above the participation types'),
        '#default_value'    => variable_get('dw_campaigns_participation_type_blurb', 'Receive a free t-shirt when you fundraise or donate a minimum of $40 per adult')
    ); 
    
    
    $form['nav_bar'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('Nav Bar Links Settings'),
        '#description'  => t('Configure Nav Bar Links')
    );
    
    $form['nav_bar']['dw_campaigns_derby_general_donation_link'] = array(
        '#title'            => 'Address for "General Donation" link',
        '#type'             => 'textfield',
        '#description'      => t('URL for GIVE link on Banner Menu (and Give a General Donation button in Derby)'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_derby_general_donation_link', '')           
    );
    $form['nav_bar']['dw_campaigns_derby_about_np_link'] = array(
        '#title'            => 'Address for "About Non-profit" link (About PWSACO / About Us)',
        '#type'             => 'textfield',
        '#description'      => t('URL for ABOUT US links on Banner menu and other ABOUT buttons.'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_derby_about_np_link', '')           
    );

// signup email tokens fieldset
    $form['signup_tokens_fieldset'] = array(
        '#type'		=> 'fieldset',
        '#title'	=> t('Signup Email Tokens'),
        //'#description'	=> t(''),
        '#collapsible'	=> TRUE,
        '#collapsed'	=> TRUE,
    );

    $form['signup_tokens_fieldset']['tokens'] = array(
        '#type'		=> 'fieldset',
        '#value'	=> getContactTokens() . getEmailTokens(),
    );


// signup email fieldset
    $form['signup'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('Signup Email'),
        '#description'  => t('Signup Email')
    );

    $form['signup']['dw_campaigns_signup_from'] = array(
        '#title'            => 'Email From Address',
        '#type'             => 'textfield',
        '#description'      => 'From Email, use format of:  user@host.com  -OR- John Smith &lt;user@host.com&gt;',
        '#default_value'    => variable_get('dw_campaigns_signup_from', '')
    );


    $form['signup']['dw_campaigns_signup_subject'] = array(
        '#title'            => 'Email Subject',
        '#type'             => 'textfield',
        '#description'      => t('Subject for Email sent upon account creation'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_signup_subject', '')           
    );

    $form['signup']['dw_campaigns_signup_body'] = array(
        '#title'            => 'Email Body',
        '#type'             => 'textarea',
        '#description'      => t('Body of Email sent upon account creation'),
        '#default_value'    => variable_get('dw_campaigns_signup_body', '')           
    );

    $form['signup']['dw_campaigns_signup_words_node'] = array(
        '#title'            => 'node number of drupal page with signup "thanks"',
        '#type'             => 'textfield',
        '#description'      => t('page to redirect to'),
        '#default_value'    => variable_get('dw_campaigns_signup_words_node', '')           
    );



// default value email tokens fieldset
    $form['default_tokens_fieldset'] = array(
	'#type'		=> 'fieldset',
	'#title'	=> t('Thank You / Receipt Tokens'),
	//'#description'	=> t(''),
	'#collapsible'	=> TRUE,
	'#collapsed'	=> TRUE,
    );

    $form['default_tokens_fieldset']['tokens'] = array(
        '#type'             => 'fieldset',
        '#value'            => getContactTokens() . getTokens(),
    );

// default values (Thank You / Receipt emails) fieldset
    $form['defaults'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Campaign Content Type Defaults'),
        '#description'      => t('Configure the default values used for new campaigns')
    );
    
    $form['defaults']['dw_campaigns_default_pcp_intro_text'] = array(
        '#title'            => 'Personal Campaign Intro Text',
        '#type'             => 'textarea',
        '#description'      => t('Default Personal Campaign Intro Text (tokens available: fundraising_campaign, user, contact)'),
        '#default_value'    => variable_get('dw_campaigns_default_pcp_intro_text', '')                   
    );

    
    $form['defaults']['dw_campaigns_default_thankyou_title'] = array(
        '#title'            => 'Thank You Title',
        '#type'             => 'textfield',
        '#description'      => t('Default Thank You Title'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_thankyou_title', '')           
    );
    
    $form['defaults']['dw_campaigns_default_thankyou_text'] = array(
        '#title'            => 'Thank You Text',
        '#type'             => 'textarea',
        '#description'      => t('Default Thank You Text'),
        '#default_value'    => variable_get('dw_campaigns_default_thankyou_text', '')           
    );
    
    $form['defaults']['dw_campaigns_default_thankyou_footer'] = array(
        '#title'            => 'Thank You Footer',
        '#type'             => 'textfield',
        '#description'      => t('Default Thank You Footer'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_thankyou_footer', '')           
    );
            
            
    $form['defaults']['dw_campaigns_default_receipt_from_name'] = array(
        '#title'            => 'Receipt From Name',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt From Name'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_receipt_from_name', '')           
    );
    
    $form['defaults']['dw_campaigns_default_receipt_from_email'] = array(
        '#title'            => 'Receipt From Email',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt From Email'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_receipt_from_email', '')           
    );


    $form['defaults']['dw_campaigns_default_cc_receipt'] = array(
        '#title'            => 'Receipt CCs',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt CCs (Carbon Copy) - comma seperated'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_cc_receipt', '')           
    );
    
    $form['defaults']['dw_campaigns_default_bcc_receipt'] = array(
        '#title'            => 'Receipt BCCs',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt BCCs (Blind Carbon Copy) - comma seperated'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_bcc_receipt', '')           
    );

    $form['defaults']['dw_campaigns_default_receipt_subject'] = array(
        '#title'            => 'Receipt Email Subject (donor)',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt Email Subject'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_receipt_subject', '')           
    );

    $form['defaults']['dw_campaigns_default_receipt_text'] = array(
        '#title'            => 'Receipt Text (donor)',
        '#type'             => 'textarea',
        '#description'      => t('Default Receipt Text'),
        '#default_value'    => variable_get('dw_campaigns_default_receipt_text', '')           
    );
    
// donation settings fieldset
    $form['notifications'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('PCP Owner Notifications'),
        '#description'      => t('Notificaiton Emails (these go to PCP owners per notify settings)')
    );
// notifications fieldset
    $form['notifications']['dw_campaigns_default_receipt_subject_pcp_owner'] = array(
        '#title'            => 'Receipt (Notification) Email Subject (pcp owner)',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt Email Subject'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_receipt_subject_pcp_owner', '')           
    );

    $form['notifications']['dw_campaigns_default_receipt_text_pcp_owner'] = array(
        '#title'            => 'Receipt (Notification) Text (pcp owner)',
        '#type'             => 'textarea',
        '#description'      => t('Default Receipt Text'),
        '#default_value'    => variable_get('dw_campaigns_default_receipt_text_pcp_owner', '')           
    );
    

    $form['notifications']['dw_campaigns_default_notification_weekly_subject_pcp_owner'] = array(
        '#title'            => 'Weekly Notification Email Subject (pcp owner)',
        '#type'             => 'textfield',
        '#description'      => t('Email Subject'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_notification_weekly_subject_pcp_owner', '')           
    );

    $form['notifications']['dw_campaigns_default_notification_weekly_text_pcp_owner'] = array(
        '#title'            => 'Weekly Notification Text (pcp owner)',
        '#type'             => 'textarea',
        '#description'      => t('Text'),
        '#default_value'    => variable_get('dw_campaigns_default_notification_weekly_text_pcp_owner', '')           
    );

    $form['notifications']['dw_campaigns_default_notification_progress_subject_pcp_owner'] = array(
        '#title'            => 'Progress Notification Email Subject (pcp owner)',
        '#type'             => 'textfield',
        '#description'      => t('Email Subject'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_notification_progress_subject_pcp_owner', '')           
    );

    $form['notifications']['dw_campaigns_default_notification_progress_text_pcp_owner'] = array(
        '#title'            => 'Progress Notification Text (pcp owner)',
        '#type'             => 'textarea',
        '#description'      => t('Text'),
        '#default_value'    => variable_get('dw_campaigns_default_notification_progress_text_pcp_owner', '')           
    );



// donation settings fieldset
    $form['donation_settings_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Default Contribution'),
        '#description'      => t('Default values for contribution related fields')
    );

    $form['donation_settings_fieldset']['default_contribution_amount'] = array(
        '#title'            => 'Suggested Contribution Amount',
        '#type'             => 'textfield',
        '#description'      => t('Default Contribution Amount'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('default_contribution_amount', '100.00')         
    );

/*
    $form['privacy_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Default Privacy Settings'),
        '#description'      => t('When someone donates to a PCP these are the default privacy settings on the form')
    );

    $form['privacy_fieldset']['privacy_anonymous']    = array(
        '#type'             => 'select',
        '#title'            => t('Make this donation anonymous to the user'),
        '#options'          => array(
            '0'             => 'No',
            '1'             => 'Yes',
        ),
        '#default_value'    => 0,
        '#required'         => true
    );

    $form['privacy_fieldset']['privacy_contact']    = array(
        '#type'             => 'select',
        '#title'            => t('Allow the user to contact me'),
        '#options'          => array(
            '0'             => 'No',
            '1'             => 'Yes',
        ),
        '#default_value'    => 1,
        '#required'         => true
    );
*/
    /**
     * Invitation text
     */

// invitation tokens fieldset
    $form['invitation_tokens_fieldset'] = array(
	'#type'		=> 'fieldset',
	'#title'	=> t('Invitation Email / Social Sharing Tokens'),
	//'#description'	=> t(''),
	'#collapsible'	=> TRUE,
	'#collapsed'	=> TRUE,
    );

    $form['invitation_tokens_fieldset']['tokens'] = array(
        '#type'		=> 'fieldset',
        '#value'	=> getTokens(),
    );


// invitation fieldset
    $form['invitation_text_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Invitation Text'),
        '#description'      => t('When someone creates their personal contribution page they will be asked to email friends and family. The text below is the default text that the invitiation email will have. The user will be able to edit this before they send it.')
    );
    
    $form['invitation_text_fieldset']['dw-campaigns-invitation-text'] = array(
        '#title'            => 'Default Text for "sharing" email',
        '#type'             => 'textarea',
        '#default_value'    => variable_get('dw-campaigns-invitation-text', '')
    );
    
    $form['invitation_text_fieldset']['dw-campaigns-invitation-text-from-email-name'] = array(
        '#title'            => 'From "name" for invitation email',
        '#type'             => 'textfield',
        '#default_value'    => variable_get('dw-campaigns-invitation-text-from-email-name', '')
    );
    
    $form['invitation_text_fieldset']['dw-campaigns-invitation-text-from-email-address'] = array(
        '#title'            => 'From Email Address for invitation email',
        '#type'             => 'textfield',
        '#default_value'    => variable_get('dw-campaigns-invitation-text-from-email-address', '')
    );
    
    $form['sharing_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Sharing Default Text'),
        '#description'      => t('Default text for sharing messages')
    );

    $form['sharing_fieldset']['dw-campaigns-facebook-text'] = array(
        '#title'            => 'Default Text for "sharing" on Facebook',
        '#type'             => 'textfield',
        '#maxlength'        => 400,
        '#default_value'    => variable_get('dw-campaigns-facebook-text', '')
    );
    
    $form['sharing_fieldset']['dw-campaigns-twitter-text'] = array(
        '#title'            => 'Default Text for "sharing" tweet',
        '#description'      => '(120 chars, so there is room for link)',
        '#type'             => 'textfield',
        '#maxlength'        => 120,
        '#default_value'    => variable_get('dw-campaigns-twitter-text', '')
    );

    return system_settings_form($form);

}

function getEmailTokens() {
    return nl2br(
'
USER - User that is logged in
{{user}{mail}}
{{user}{name}}
{{user}{pass}}
{{user}{displayname}}

ORG - Organization values (From admin module)
{{org}{name}}
{{org}{name_abbreviated}}
{{org}{homepageurl}}
{{org}{website}}
');
    
}

function getContactTokens() {
    return nl2br(
'CONTACT - information about the user registering, or donator donating
{{contact}{contact_id}}
{{contact}{sort_name}}
{{contact}{display_name}}
{{contact}{first_name}}
{{contact}{last_name}}
{{contact}{street_address}}
{{contact}{supplemental_address_1}}
{{contact}{city}}
{{contact}{postal_code}}
{{contact}{state_province_name}}
{{contact}{state_province}}
{{contact}{country}}
{{contact}{phone}}
{{contact}{email}}
{{contact}{world_region}}
');
   
}

function getTokens() {
    return nl2br(
'
FUNDRAISING_CAMPAIGN - these are the values from the derby / walk content
{{fundraising_campaign}{type}}
{{fundraising_campaign}{uid}}
{{fundraising_campaign}{status}}
{{fundraising_campaign}{created}}
{{fundraising_campaign}{changed}}
{{fundraising_campaign}{title}}
{{fundraising_campaign}{body}}
{{fundraising_campaign}{teaser}}
{{fundraising_campaign}{revision_timestamp}}
{{fundraising_campaign}{name}}
{{fundraising_campaign}{field_dw_fundraising_goal}}
{{fundraising_campaign}{fundraising_goal_formatted}}
{{fundraising_campaign}{raised_amount}}
{{fundraising_campaign}{field_dw_campaign_status}}
{{fundraising_campaign}{field_dw_event_date}}
{{fundraising_campaign}{field_dw_event_time}}
{{fundraising_campaign}{field_dw_event_location}}
{{fundraising_campaign}{field_dw_address}{city}}
{{fundraising_campaign}{field_dw_address}{street}}
{{fundraising_campaign}{field_dw_address}{postal_code}}

CONTRIBUTION - values from a donation
{{contribution}{id}}
{{contribution}{contact_id}}
{{contribution}{contribution_type_id}}
{{contribution}{contribution_page_id}}
{{contribution}{payment_instrument_id}}
{{contribution}{receive_date}}
{{contribution}{total_amount}}
{{contribution}{invoice_id}}
{{contribution}{currency}}
{{contribution}{source}}

USER - User that is logged in
{{user}{uid}}
{{user}{name}}
{{user}{pass}}
{{user}{mail}}
{{user}{displayname}}
{{user}{created}}
{{user}{access}}
{{user}{login}}
{{user}{status}}
{{user}{timezone}}
{{user}{hostname}}
{{user}{timestamp}}
{{pcp}{title}}
{{pcp}{intro_text}}
{{pcp}{page_text}}
{{pcp}{donate_link_text}}
{{pcp}{contribution_page_id}}
{{pcp}{goal_amount}}
{{pcp}{raised_amount}}
{{pcp}{donation_count}}
{{pcp}{url}}

PCP_CONTACT - fundraising page owner (civicrm info)
{{pcp_contact}{contact_id}}
{{pcp_contact}{contact_type}}
{{pcp_contact}{sort_name}}
{{pcp_contact}{display_name}}
{{pcp_contact}{first_name}}
{{pcp_contact}{last_name}}
{{pcp_contact}{street_address}}
{{pcp_contact}{city}}
{{pcp_contact}{postal_code}}
{{pcp_contact}{geo_code_1}}
{{pcp_contact}{geo_code_2}}
{{pcp_contact}{state_province_id}}
{{pcp_contact}{state_province_name}}
{{pcp_contact}{state_province}}
{{pcp_contact}{country}}
{{pcp_contact}{phone}}
{{pcp_contact}{email}}

PCP_USER = fundfaising page owners (drupal info; used for displayname)
{{pcp_user}{uid}}
{{pcp_user}{name}}
{{pcp_user}{pass}}
{{pcp_user}{mail}}
{{pcp_user}{displayname}}
{{pcp_user}{created}}
{{pcp_user}{access}}
{{pcp_user}{login}}
{{pcp_user}{status}}
{{pcp_user}{timezone}}
{{pcp_user}{hostname}}
{{pcp_user}{timestamp}}

ORG - Organization values (From admin module)
{{org}{name}}
{{org}{name_abbreviated}}
{{org}{homepageurl}}
{{org}{website}}
');
    
}

function dw_campaigns_reports_email() {

    $all = array();

    $result = db_query("SELECT mail FROM users");
    while ($users = db_fetch_object($result))
    {
      $clean_email = trim($users->mail);
      if(strstr($clean_email, "@") === FALSE) {
          continue;
      }

      $all[] = $clean_email;
    }

    $emails = implode("\n", $all);

    return nl2br($emails);
}

?>
