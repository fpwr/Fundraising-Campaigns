<?php
    global $options;

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
    //$location_create_link_words = t('Register Now') . '<span class="register-tiny">' . t('and create your page') . '</span>';
    $location_create_link_words = t('Register Now');

    // if we don't have a user, lets guide them through the process
    if($user->uid <= 0) {
        $location_create_url    = '/dw/user/login?create=1';
        //$location_create_url    = '/dw/user/register';
        $location_create_url    = '/dw/user/register_oss';
    } else {
        $location_create_link_words = 'Create My Page';    
    }

    if(empty($campaign_id) && $user->uid > 0) {
       
        //$location_create_class  = 'location-not-found';
        $location_create_class  = '';
	$location_create_url    = '/dw/user/edit_page';
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

    // 350 x 245 is about the size of the box
    $background_image = '';
    if(!is_null($campaign)) {
	$temp_filename = $campaign->field_dw_campaign_image[0]['filepath'];
        if(file_exists($temp_filename)) {
            $image_params = array(
                'w' => 620,
            );
            $thumb = $image = _dw_campaigns_thumb($temp_filename, $image_params);
            $background_image = "background:transparent url('$thumb') no-repeat left top;";
        }

        $vid = dw_campaigns_get_youtube($campaign->field_dw_campaign_youtube[0]['value']);
    }    

    $contest_link = '/content/oss-contest-rules';

global $language;
if($language->language != 'en') {
    $contest_link = '/node/93';
}

?>
<script type="text/javascript">
$(document).ready(function() {
    
    var my_loc = <?php echo $my_location;?>;
    
    $(".location-not-found").click(function(){
        alert('Please Select a Location from the Find a Walk Menu before Creating A Fundraising Page');
        return false;
    });
    
    $(".location-mismatch").click(function(e){
        var res = confirm('<?php echo t('You already have a Walking Campaign for a Different Location - Click OK to Go to your Walk Location'); ?>');
        if(res) {
            window.location = "/dw/walking/location/" + my_loc 
        }
        return false;
    });

});

</script>

<div class="support-left" style="<?php echo $background_image; ?>">
<?php 
if(!empty($vid)) {
    printf('<iframe class="youtube-video" width="393" height="325" src="http://www.youtube.com/embed/%s?rel=0" frameborder="0" allowfullscreen></iframe>', $vid);
}
?>
</div>
<div class="support-right">
    <div class="right">
        <ul>
            <li class="raise">
                <h2 class="raise-money-location"><?php echo t('Create a Fundraising page'); ?></h2>
                <span class="raise-words"><?php echo t('Create a Fundraising Page and Register Now'); ?></span>
                <a href="<?php echo $location_create_url;?>" class="btn <?php echo $location_create_class; ?>"><?php echo $location_create_link_words;?></a>
            </li>
            <li class="support">
                <h2 class="support-the-cause"><?php echo t('Donate to a Walker'); ?></h2>
                <p><?php echo t('Support a participant in this walk.'); ?></p>
                <form action="<?php echo $search_url; ?>" method="post">
<?php
    $formId             = 'dw_campaigns_user_search_dummy_form';
    $form               = dw_campaigns_user_search_dummy_form($campaign_id);
    $form['#build_id']  = 'form-'. md5(uniqid(mt_rand(), true));

    if(count($_POST) > 0) {
        $form['#post'] = $_POST;
    }

    $formState         = array('storage' => NULL, 'submitted' => FALSE);

    drupal_prepare_form($formId, $form, $formState);
    drupal_process_form($formId, $form, $formState);

    echo drupal_render($form['query']);
    echo drupal_render($form['form_id']);
    echo drupal_render($form['form_build_id']);
    echo drupal_render($form['submit']);
?>
                </form>

            </li>
            <li class="city">
                <!-- <h2 class="raise-money-host"></h2> -->
                <span><?php echo t('Don\'t see your city in the list? !urlstart Click here to host a Walk !urlend in Your City', array('!urlstart' => '<a href="/dw/walking/host" class="">', '!urlend' => '</a>')); ?></span>
            </li>
            <li class="contest">
                <!-- <a href="/content/oss-contest-rules"><?php echo t('Join Our May Awareness Campaign'); ?></a> <span class="content-words"><?php echo t('Find out how you can win an iPAD 2!'); ?></span> -->
                <a href="<?php echo $contest_link; ?>"><?php echo t('Win a Trip for 2 or an xBox 360 with Kinect<br>PWS Parents:  Win a trip to our Conference!'); ?></a>
            </li>
    </div>
</div>
