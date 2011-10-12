<?php
    $mode_type      = dw_campaigns_get_selected_type();

    $hasPCP         = FALSE;

    if($mode_type != 'walking') {     
        $campaign_id    = dw_campaigns_get_default_campaign();
        $campaign       = node_load($campaign_id);
        
        $pcps = _dw_campaigns_get_pcps_for_campaign($campaign);
        foreach($pcps as $pcp) {            
            $drupal_id      = _dw_campaigns_contact_id_get_user($pcp->contact_id);
            if($drupal_id == $user->uid) {
                $hasPCP = TRUE;
            }
        }
    } else {
        $campaign       = dw_campaigns_get_selected_campaign();
        $campaign_id    = $campaign->nid;
        
        $hasPCP = false;   
        $my_location    = '-1';
    
        if($campaign == '') {
            $search_url     = '/dw/campaign/current/search';
        } else {
            $search_url     = dw_campaigns_get_campaign_path($campaign_id, '/dw/campaign', 'search');    
            }
    
        $pcps = _dw_campaigns_get_pcp_by_drupal_id($user);
    
        // todo - fix this so it works 'next year'
        foreach($pcps as $key => $pcp) {
           $hasPCP = TRUE;
           $dummy = new stdClass;
           $dummy->id = $pcp->contribution_page_id;
           $myCampaign = dw_campaigns_get_campaign_from_contribution_page($dummy);
           if(!is_null($myCampaign)) {
               $my_location = $myCampaign->nid;
           }
           break;
        }
    }
    
    if(is_null($campaign)) {
        return;
    }
    
    $location_create_url = '#';
    $location_create_class  = 'location-found';
    
    if(!is_null($campaign)) {
        $location_text          = $campaign->field_dw_campaign_location[0]['value'];
    } else {
        $location_text = ''; // "Select a Location from the 'Find a Walk' menu";
    }

    // default behavior    
    $location_create_url        = "/dw/walking/start/$campaign_id";
    $location_create_link_words = t('Create A Fundraising Page Now');
    $location_create_link_words = t('Register Now');

    // if we don't have a user, lets guide them through the process
    if($user->uid <= 0) {
        $location_create_url    = '/dw/user/login?create=1';
        $location_create_url    = '/dw/user/register';
        if($mode_type == 'walking') {
            $location_create_url    = '/dw/user/register_oss';
        }
    }

    if(empty($campaign_id) && $user->uid > 0) {
        $location_create_class  = 'location-not-found';
        $location_create_url    = '#';
    }

 
    if($hasPCP && ($hasPCP && ($campaign_id == $my_location) ) ) {
        // we are on the right page
        $location_create_url        = "/dw/user/edit_page";            
        $location_create_link_words = t('Edit My Page');
    } elseif ($hasPCP && ($campaign_id != $my_location)) {
        // we are on the wrong page - we do the class in the middle of the page currently :)
        $location_create_url        = "/dw/user/edit_page";            
        $location_create_link_words = t('Edit My Page');
        $location_create_class  = "location-mismatch";    
        $location_create_class  = '';
    }
?>


    <div class="join-the-race">
        <h2><?php echo t('Join the race'); ?></h2>
        <p><?php echo t('It\'s not too late to join the race and raise money for the event.'); ?></p>
        <a href="<?php echo $location_create_url;?>" class="btn <?php echo $location_create_class; ?>"><?php echo $location_create_link_words;?></a>
    </div>
