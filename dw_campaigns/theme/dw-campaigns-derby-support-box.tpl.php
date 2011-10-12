<?php
    $hasPCP         = FALSE;

    $campaign_id    = dw_campaigns_get_default_campaign();
    $campaign       = node_load($campaign_id);
    
    $create_url     = "/dw/derby/start/$campaign_id";
    
    
    $event_url = '';
    $event_id   = dw_campaigns_get_event_page_id_from_campaign($campaign);

    if(is_numeric($event_id) && $event_id > 0) {
        $baseurl    = variable_get('dw_campaigns_cfg_base_url', '');
        if(!empty($baseurl)) {
            $event_url    = sprintf("%s/civicrm/event/info?id=%s&reset=1", $baseurl, $event_id);
            $event_url    = sprintf("%s/civicrm/event/register?id=%s&reset=1", $baseurl, $event_id);
        }
    }
    
    
    $give_url           = variable_get('dw_campaigns_derby_general_donation_link', '');
    
?>
<script type="text/javascript">
$(document).ready(function() {
    $(".racer-select-go").click(function(){
        var picked = $('#racer-select').val();
        if (picked == '') {
            alert('<?php echo t('Please select a racer'); ?>');
        } else {
            window.location.href = picked;
        }
    });
});
</script>
<div class="support-left">
    <?php
        if(!empty($event_url)) {
    ?>
    <?php echo t('Planning to attend the derby?'); ?>
    <?php echo l(t('Register Now!'), $event_url); ?>
    <?php
        }
    ?>
</div>
<div class="support-right">
    <div class="left">
        <h2><?php echo t('Support the cause'); ?></h2>
        <p><?php echo t('Give a general donation to this event or find a current fundraiser to support.'); ?></p>
        
        <select name="racer" id="racer-select" class="form-select">
            <option value=""><?php echo t('Select a Racer\'s Page'); ?></option>
            <?php
                
                $pcps = _dw_campaigns_get_pcps_for_campaign($campaign);
                foreach($pcps as $pcp) {            
                    $drupal_id      = _dw_campaigns_contact_id_get_user($pcp->contact_id);
                    if($drupal_id == $user->uid) {
                        $hasPCP = TRUE;
                    }
                    $fake_user      = user_load(array('uid'=>$drupal_id));
                    $params = array(
                        'contact_id'    => $pcp->contact_id,
                        'returnFirst'   => 1
                    );

                    $contact    = _dw_civicrm_contact_get($params);
                    $url        = dw_campaigns_user_get_pcp_url($fake_user, $campaign, TRUE);
                    $pcp_members[$url]  = $fake_user->displayname;
                //    printf('<option value="%s" style="display:block">%s - %s, %s</option>', $url, $fake_user->displayname, $contact->city, $contact->state_province);
                //    printf('<option value="%s" style="display:block">%s</option>', $url, $fake_user->displayname);
     
                }

                foreach($pcp_members as $key => $value) {
                    $temp_arr[$key] = strtolower($value);
                }

                asort($temp_arr, SORT_STRING);
                foreach($temp_arr as $url => $ignored) {
                    // loop through sorted list, but use display name from original list
                    printf('<option value="%s" style="display:block">%s</option>', $url, $pcp_members[$url]); 
                }
            ?>
        </select>
        <a class="sbtn racer-select-go"><span>Go</span></a>
        <?php
        if(!empty($give_url)) {
        ?>
            <a href="<?php echo $give_url; ?>" class="general-donation btn"><span><?php echo t('Give a General Donation to PWSACO'); ?></span></a>
        <?php
        }
        ?>
    </div>
    <div class="right">
        <h2><?php echo t('Join the race'); ?></h2>
        <p><?php echo t('It\'s not too late to join the race and raise money for the event.'); ?></p>
<?php
    if($user->uid>0) {
        if($hasPCP) {
            echo '<a href="/dw/user/edit_page">' . t('Edit My Fundraising Page') . '</a>';
        } else {
            echo '<a href="' . $create_url . '">' . t('Create a fundraising page now') . '</a>';
        }
    } else {
        echo '<a href="/dw/user/login?create=1" class="btn">' . t('Create a fundraising page now') . '</a>';
    }
?>
    </div>
</div>
