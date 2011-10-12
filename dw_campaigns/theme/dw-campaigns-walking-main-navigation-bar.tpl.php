<?php
    global $selected;
    global $language;

    $campaign_id = $selected;
 
    $about_url  = variable_get('dw_campaigns_derby_about_np_link', '#');
    if($language->language != 'en') {
        $about_url = '/node/86';
    }
    $give_url   = variable_get('dw_campaigns_derby_general_donation_link', '');
    $derby_url  = '';
    //$leaderboard_url  = '/dw/campaign/current/leaderboard';

    $leaderboard_url  = '/dw/campaign/current/leaderboard';
    if($selected != -1 && $selected != '' && !(arg(2) == 'give' && arg(3) == 'general-donation')) {
   
        $leaderboard_url  = dw_campaigns_get_campaign_path($selected, '/dw/campaign', 'leaderboard');
    }

    // TODO - lookup homepage campaign - this may need to be changed to be like 'leaderboard' with current
    $toplocations_url  = '/dw/walking/toplocations';

    
    $active     = 'active-path';
    
    $home_class         = '';
    $leaderboard_class  = '';
    $account_class      = '';

    $body_class         = dw_campaigns_make_body_class();
    if($body_class == 'dw-walking') {
        $home_class         = $active;
    } elseif (arg(3) == 'leaderboard') {
        $leaderboard_class  = $active;
    } elseif (arg(1) == 'user') {
        $account_class      = $active;
    } elseif ($body_class == 'dw-walking-toplocations') {
        $toplocations_class = $active;
    }
?>
<div class="nav-bar">
    <ul>
        <li class="<?php echo $home_class; ?>"><a href="/dw/walking"><?php echo t('Home'); ?></a></li>
        
        <li class="<?php echo $leaderboard_class; ?>"><a href="<?php echo $leaderboard_url; ?>"><?php echo t('Leader Board'); ?></a></li>
        <li class="<?php echo $toplocations_class; ?>"><a href="<?php echo $toplocations_url; ?>"><?php echo t('Locations'); ?></a></li>
        

        <li><a href="<?php echo $about_url; ?>"><?php echo t('About Us'); ?></a></li>

        <?php if(!empty($give_url)) {
        ?>
        <li><a href="<?php echo $give_url; ?>" target="_BLANK"><?php echo t('Give'); ?></a></li>
        <?php
        }
        ?>
        
        <?php if(!empty($derby_url)) {
        ?>
        <li><a href="<?php echo $derby_url; ?>"><?php echo t('About the Walk'); ?></a></li>
        <?php
        }
        ?>
        <?php
        if($user->uid != 0) {
        ?>
        
        <li class="<?php echo $account_class; ?> last"><a href="/dw/user/profile"><?php echo t('My Account'); ?></a></li>
        <?php
        }
        ?>
    </ul>
</div>
