<?php
    if($pcp->id==0) {
        return;    
    }
echo "<!-- PCP_ID: {$pcp->id} -->";

    $mode_type      = dw_campaigns_get_selected_type();

    $image_params   = array(
        'w'                 => 260,
    );
    $matched_image  = '';
    //get the user and drop it in for a preview
    $imageSrc       = _dw_campaigns_get_photo($thisUser,  $image_params, 'pcp-photo', $pcp->id, $matched_image);
    
    $donate_url     = dw_campaigns_get_donate_url($thisUser, $campaign);

    $params = dw_campaigns_get_merge_object($campaign, $pcp);
    $rawurl = dw_campaigns_user_get_pcp_url($thisUser, $campaign, TRUE);

    $url = urlencode($rawurl);
    $text = urlencode( t("Help me support !title", array('!title' => $campaign->title)) );
    $twitter_text = variable_get('dw-campaigns-twitter-text', $text);
    $facebook_text = variable_get('dw-campaigns-facebook-text', $text);

    dw_campaigns_do_merge($twitter_text, $params);
    dw_campaigns_do_merge($facebook_text, $params);
    dw_campaigns_do_merge($text, $params);
    dw_campaigns_do_merge($form['invitation-text'], $params);

    $extra          = _dw_campaigns_get_pcp_extra($pcp->id);
    $vid            = dw_campaigns_get_youtube($extra->youtube_url);
?>
<div class="users-pcp-page-left">
    <h2><?php echo $pcp->title;?></h2>
<?php
    if(empty($vid)) {
?>
    <img src="<?php echo $imageSrc; ?>">
<?php
    } else {
        printf('<iframe class="youtube-video" width="260" height="225" src="http://www.youtube.com/embed/%s?rel=0" frameborder="0" allowfullscreen></iframe>', $vid);
    }
?>
</div>
<div class="users-pcp-page-right">
    <div class="share-this-cause">
        <ul>
            <li><?php echo t('Share this cause'); ?></li>
            <li class="addthis_toolbox addthis_default_style">
                <a class="addthis_button_email"></a>
            </li>
            <li>
                <a class="facebook_share" target="_blank" href="http://www.facebook.com/sharer.php?u=<?php echo $url; ?>&t=<?php echo $facebook_text; ?>"> <img src="http://facebook.com/images/connect_favicon.png"></a>
            </li>
            <li>
                <a class="twitter_share" target="_blank" href="http://twitter.com/share?url=<?php echo $url; ?>&text=<?php echo $twitter_text; ?>"> <img src="http://twitter.com/images/goodies/tweetn.png"></a>
            </li>
            <li>
                <iframe style="overflow: hidden; border: 0px none; width: 82px; height: 25px;" src="//www.facebook.com/plugins/like.php?href=<?php echo $url; ?>&amp;layout=button_count&amp;show_faces=false&amp;width=100&amp;action=like&amp;font=arial&amp;layout=button_count"></iframe>
            </li>
        </ul>
        <script type="text/javascript" src="http://s7.addthis.com/js/250/addthis_widget.js#username=xa-4d587a2e68371ba0"></script>
    </div>
    <p><?php echo $pcp->intro_text;?></p>
    <div class="progress-bars">
        <div class="left">
            <?php echo theme('dw_campaigns_derby_pcp_statistics', $thisUser, $campaign, $pcp); ?>
        </div>
        <div class="right">
            <a href="/<?php echo $donate_url;?>" class="btn btn-yellow"><?php echo t('Donate Now!'); ?></a>
        </div>
    </div>
</div>
