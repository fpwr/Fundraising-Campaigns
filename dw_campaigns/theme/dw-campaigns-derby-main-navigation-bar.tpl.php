<?php

    $campaign_id        = dw_campaigns_get_default_campaign();

    
    $about_url          = variable_get('dw_campaigns_derby_about_np_link', '#');
    $give_url           = variable_get('dw_campaigns_derby_general_donation_link', '');
    //$derby_url  = dw_campaigns_get_campaign_path('4');
    $derby_url          = variable_get('dw_campaigns_derby_about_derby', '');

    $leaderboard_url    = dw_campaigns_get_campaign_path($campaign_id, '/dw/campaign', 'leaderboard');
    // TODO - lookup homepage campaign
    $search_url         = dw_campaigns_get_campaign_path($campaign_id, '/dw/campaign', 'search');

    $active             = 'active-path';


    $home_class         = '';

    $leaderboard_class  = '';
    $account_class      = '';


    if(dw_campaigns_make_body_class() == 'dw-derby') {
        $home_class         = $active;
    } elseif (arg(3) == 'leaderboard') {
        $leaderboard_class  = $active;
    } elseif (arg(1) == 'user') {
        $account_class      = $active;
    }

?>
<div class="nav-bar">
    <ul>
        <li class="<?php echo $home_class; ?>"><a href="/dw"><?php echo t('Home'); ?></a></li>
        <li><a href="<?php echo $about_url; ?>" target="_BLANK""><?php echo t('About PWSACO'); ?></a></li>

        <?php if(!empty($give_url)) {
        ?>
        <li><a href="<?php echo $give_url; ?>">Give</a></li>
        <?php
        }
        ?>

        <li><a href="<?php echo $derby_url; ?>"><?php echo t('About the Derby'); ?></a></li>
        <li class="<?php echo $leaderboard_class; ?>"><a href="<?php echo $leaderboard_url; ?>"><?php echo t('Leader Board'); ?></a></li>
        <?php
        if($user->uid != 0) {
        ?>
        <li class="<?php echo $account_class; ?>"><a href="/dw/user/profile"><?php echo t('My Account'); ?></a></li>
        <?php
        }
        ?>

        <li class="search"><form method="post" action="<?php echo $search_url; ?>"><input type="text" name="query" class="nav-search form-text has-default-text" value="search for a fundraiser"><input type="submit" value="<?php echo t('Go'); ?>" class="form-submit"></form></li>
    </ul>
</div>
