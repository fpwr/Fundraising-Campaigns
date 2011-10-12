<?php
    if(dw_campaigns_get_selected_type()!='walking') {
        $pcp = dw_campaigns_user_get_pcp_for_campaign($user, $campaign);
    } else {
        $pcp = dw_campaigns_user_get_pcp_for_walking($user);
    }
    if(is_null($pcp->id)) {
        echo t('you do not have any active personal campaign pages');
        return;
    }

    $params = dw_campaigns_get_merge_object($campaign, $pcp);
    $rawurl = dw_campaigns_user_get_pcp_url($user, $campaign, TRUE);

    $url = urlencode($rawurl);
    $text = urlencode( t('Help me support !title', array('!title' => $campaign->title)) );
    $twitter_text = variable_get('dw-campaigns-twitter-text', $text);
    $facebook_text = variable_get('dw-campaigns-facebook-text', $text);

    dw_campaigns_do_merge($twitter_text, $params);
    dw_campaigns_do_merge($facebook_text, $params);
    dw_campaigns_do_merge($text, $params);
    dw_campaigns_do_merge($form['invitation-text'], $params);
?>

<a class="twitter_share" target="_blank" href="http://twitter.com/share?url=<?php echo $url; ?>&text=<?php echo $twitter_text; ?>"> <img src="http://twitter.com/images/goodies/tweetn.png"><span><?php echo t('Tweet on Twitter'); ?></span></a>
<a class="facebook_share" target="_blank" href="http://www.facebook.com/sharer.php?u=<?php echo $url; ?>&t=<?php echo $facebook_text; ?>"> <img src="http://facebook.com/images/connect_favicon.png"><span><?php echo t('Share on Facebook'); ?></span></a>



        
<?php
if(!is_null(dw_campaigns_get_best_contact_id($user)) && $showEmail) {
?>
<form class="login" action="<?php echo request_uri(); ?>" method="post">

    <h2><?php echo t('Tell a friend by email'); ?></h2>
    
    <p>
        <?php echo t('Tell your friends about this personal contribution page and encourage them to visit it and support the cause.'); ?>

        <?php echo t('Please feel free to edit the text below.'); ?>
    </p>
<?php
 
    //from name
    echo drupal_render($form['invitation-from-name']);
    //invitation textarea
    echo drupal_render($form['invitation-text']);
    

    echo '<div class="share-emails">';
    echo drupal_render($form['invitation-targets']);
/*
    //emails, only show this if we have a valid user (we can reuse this )

    $index = 0;
    
    while($email = $form['email-' . $index]) {
        echo drupal_render($form['first_name-' . $index]);
        echo drupal_render($form['last_name-' . $index]);
        echo drupal_render($email);
        $index++;
    }
*/
    echo '</div>'; 

    echo drupal_render($form['form_id']);
    echo drupal_render($form['form_build_id']);
    echo drupal_render($form['submit']);


}
?>

</form>
