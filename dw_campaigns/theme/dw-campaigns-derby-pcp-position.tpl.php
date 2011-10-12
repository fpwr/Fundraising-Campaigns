<?php
    if(arg(1) != 'users') {
        return;
    }
    // we get $thisUser, $campaign, $pcp
    
    $res                    = _dw_campaigns_campaign_find_position_of_user($campaign, $thisUser);
    // sets $postion and $total
    extract($res);  
    
    $position_name          = position_to_name($position);
    
    // I'm not sure if its possible to be in position 0 (like on a fresh campaign)
    if($position <= 1) {
        // we don't do anything, we are in the lead
        $totalBehind        = -1;
    } else {
        $previousPosition   = _dw_campaigns_campaign_get_position($campaign, $position - 1);
        $totalBehind        = $previousPosition['total'] - $total;
    }
    
    $total                  = dw_campaigns_force_decimal($total, $campaign->field_dw_currency['0']['value']);
?>

<div class="how-are-we-doing">
    <h2>How are we doing?</h2>
    <span class="words"><?php echo t('@displayname currently sits in', array('@displayname' => $thisUser->displayname)); ?></span>
    <div class="yellow-box">
        <div class="left">
    <?php
        if($position_name != 'N/A') {
            echo '<span class="position-number">' . $position_name . '</span> <span class="position-place">Place</span>';
        } else {
            echo t('Not Ranked Yet');
        }
    ?>
        </div>
        <div class="right">
            <span class="with">with</span><span class="money"><?php echo $total;?></span><span class="donations">in donations</span>
        </div>
    </div>
    <div class="below">
    <?php
        if($position > 1 && $totalBehind > 0) {
    ?>
    <?php
        $mode_type = dw_campaigns_get_selected_type();
        if($mode_type == 'walking') {
    ?>
        <span class="behind"><?php echo dw_campaigns_force_decimal($totalBehind, $campaign->field_dw_currency['0']['value']); ?></span> behind <?php echo position_to_name($position - 1); ?> place<br>
    <?php
        } else {
    ?>
        <span class="behind"><?php echo dw_campaigns_force_decimal($totalBehind, $campaign->field_dw_currency['0']['value']); ?></span> behind the <?php echo position_to_name($position - 1); ?> team<br>
    <?php
        }
    ?>
        <a href="<?php echo $previousPosition['url']; ?>"><?php echo $previousPosition['name']; ?></a>
    <?php		
        }
    ?>
    </div>
</div>
