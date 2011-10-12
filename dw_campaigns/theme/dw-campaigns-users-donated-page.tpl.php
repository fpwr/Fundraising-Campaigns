<?php
    $node = $campaign;   
 
    $event_url  = NULL;
    $event_id   = dw_campaigns_get_event_page_id_from_campaign($node);


    $mode_type  = dw_campaigns_get_selected_type();
   
    if(is_numeric($event_id) && $event_id > 0) {
        $baseurl    = variable_get('dw_campaigns_cfg_base_url', '');
        if(!empty($baseurl)) {
            $event_url    = sprintf("%s/civicrm/event/info?id=%s&reset=1", $baseurl, $event_id);
            $event_url    = sprintf("%s/civicrm/event/register?id=%s&reset=1", $baseurl, $event_id);
        }
    }
    $details = $node->teaser;
    if(strlen($details)>150) {
        $details = substr($details, 0, 150) . '...';
    }
    
    if($user->uid>0) {
        $get_started_url    = "/dw/derby/start/$campaign->nid";
    }
    else {
        $get_started_url    = '/dw/user/login?create=1';
        if($mode_type == 'walking') {
            $get_started_url    = '/dw/user/register_oss';
        }
    }
    
    
    if($mode_type == 'walking') {
        //$get_more_info_url  = 'http://www.fpwr.ca/get-involved/';
        $volunteer_email    = variable_get('dw_campaigns_host_submit_email', 'admin@');

        if(!empty($campaign->field_dw_contact_email['0']['value'])) {
            $volunteer_email    = $campaign->field_dw_contact_email['0']['value'];
        }

        $get_more_info_url  = 'mailto:' . $volunteer_email . '?subject=Volunteer%20Request';
    } else {  
        $get_more_info_url  = 'http://pwsaco.org/get-involved';
    }

    $donate_url     = dw_campaigns_get_donate_url($thisUser, $campaign);

    $params = dw_campaigns_get_merge_object($campaign, $pcp);
    $rawurl = dw_campaigns_user_get_pcp_url($thisUser, $campaign, TRUE);

    $url = urlencode($rawurl);
    $text = urlencode( t('Help me support !title', array('!title' => $campaign->title)) );
    $twitter_text = variable_get('dw-campaigns-twitter-text', $text);
    $facebook_text = variable_get('dw-campaigns-facebook-text', $text);

    dw_campaigns_do_merge($twitter_text, $params);
    dw_campaigns_do_merge($facebook_text, $params);

    if(isset($_SESSION['softContribution']) && ($pcp->id == $_SESSION['softContribution']->pcp_id)) {
        if($_SESSION['softContribution']->contribution_id == $_SESSION['contribution']->id) {
        
            $pcp = dw_campaigns_user_get_pcp_for_campaign($thisUser, $campaign);
            if(is_null($pcp->id)) {
                echo t('you do not have any active personal campaign pages');
                return;
            }
        
            $params                 = dw_campaigns_get_merge_object($campaign, $pcp);
            $params['contribution'] = $_SESSION['contribution'];
            
            $ty_title   = $campaign->field_dw_thankyou_title[0]['value'];
            $ty_text    = $campaign->field_dw_thankyou_text[0]['value'];
            $ty_footer  = $campaign->field_dw_thankyou_footer[0]['value'];

            dw_campaigns_do_merge($ty_title, $params);
            dw_campaigns_do_merge($ty_text, $params);
            dw_campaigns_do_merge($ty_footer, $params);
            
    
?>
            <div class="thankyou">
                <h2><?php echo $ty_title; ?></h2>
                <div class="thankyou-body">
                    <?php echo $ty_text; ?>
                </div>
                <div class="thankyou-footer">
                    <?php echo $ty_footer; ?>
                </div>
            </div>
<?php
        }
    }

?>
            
            <div class="donated-share donate-box">
                <h2><?php echo t('Spread the Word'); ?></h2>
                <p><?php echo t('Tell your friends about our event.'); ?></p>
                <ul>
                    <li>
                        <a class="facebook_share" target="_blank" href="http://www.facebook.com/sharer.php?u=<?php echo $url; ?>&t=<?php echo $facebook_text; ?>"> <img src="http://facebook.com/images/connect_favicon.png"></a><p><?php echo t('Have a facebook account? Tell all your friends about our event and encourage them to donate!'); ?></p>
                    </li>
                    <li>
                        <a class="twitter_share" target="_blank" href="http://twitter.com/share?url=<?php echo $url; ?>&text=<?php echo $twitter_text; ?>"> <img src="http://twitter.com/images/goodies/tweetn.png"></a><p><?php echo t('Tweet about it!  Complete this Tweet and we will help you post it!'); ?></p>
                    </li>
                    <li class="addthis_toolbox addthis_default_style" addthis:url="http://<?php echo $_SERVER['HTTP_HOST'];?><?php echo dw_campaigns_get_campaign_path($campaign->nid, '/dw/users/' . $thisUser->name); ?>">
                        <a class="addthis_button_email"></a><p><?php echo t('Send a link to our website to your friends.'); ?></p>
                    </li>
                </ul>
                <script type="text/javascript" src="http://s7.addthis.com/js/250/addthis_widget.js#username=xa-4d587a2e68371ba0"></script>
            </div>
            
            <div class="event donate-box">
                <h2><?php echo t('Join Us At The Event'); ?></h2>
                <p><?php echo t('We would love for you to attend the event in person.'); ?> 
                <table>
                    <tr class="name">
                        <th><?php echo t('Name'); ?></th><td><?php echo $node->title; ?></td>
                    </tr>
                    <tr class="date">
                        <th><?php echo t('Date'); ?></th><td><?php echo $node->field_dw_event_date[0]['value'];?> <?php echo $node->field_dw_event_time[0]['value']; ?></td>
                    </tr>
                    <tr class="location">
                        <th><?php echo t('Location'); ?></th><td><?php echo $node->field_dw_event_location[0]['value']; ?></td>
                    </tr>
<!--
                    <tr class="address">
                        <th><?php echo t('Address'); ?></th><td>123 street  Beverly Hills, CA 90210</td>
                    </tr>
-->
                    <tr class="details">
                        <th><?php echo t('Details'); ?></th><td><?php echo $details; ?></td>
                    </tr>
                    <?php
                    if(!is_null($event_url) && $mode_type != 'walking') {
                    ?>
                    <tr class="link">
                        <th><?php echo t('Register'); ?></th><td>
                        <a href="<?php echo $event_url;?>" target="_blank"><?php echo t('Register for Event'); ?></a>
                        </td>
                    </tr>
                    <?php
                    }
                    ?>
                </table>
            </div>

            <div class="get-involved donate-box">
                <h2><?php echo t('Get Involved'); ?></h2>
                <p><?php echo t('Please consider helping us in one of the following ways:'); ?></p>
                <ul>
                    <li><?php echo t('1. Create a Fundraising page of your own if you don\'t already have one. !urlstart Get Started Now! !urlend', array('!urlstart' => '<a href="' . $get_started_url . '">', '!urlend' => '</a>')); ?> </li>
                    <li><?php echo t('2. Volunteer at our event.  !urlstart Get More Info !urlend', array('!urlstart' => '<a href="' . $get_more_info_url . '">', '!urlend' => '</a>')); ?></li>
                    <li><?php echo t('3. Tell your friends and family about our organization and event. Use the Spread the Word links.'); ?></li>
                </ul>
            </div>
            
            <div class="donated-nav">
                <a class="btn" href="<?php echo dw_campaigns_get_campaign_path($campaign->nid, '/dw/users/' . $thisUser->name); ?>"><?php echo t('Take me back to the Fundraiser\'s page'); ?></a>
                <a class="btn" href="/dw/"><?php echo t('Take me to the Home Page'); ?></a>
            </div>
