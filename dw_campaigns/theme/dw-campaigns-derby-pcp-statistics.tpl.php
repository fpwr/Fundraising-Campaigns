<?php
    // we get $thisUser, $campaign, $pcp
 
    $res = dw_campaigns_get_contribution_total_for_pcp($pcp);
    $pcpTotal     = $res['total'];
    $count        = $res['count'];

    $goalTotal    = $pcp->goal_amount;
    $goalProgress = $pcpTotal;

    if($goalTotal == 0) {
        return;
    }

    // I assume we want to round 99.9 down so that we don't say 100% too soon
    $goalPercent	= floor($goalProgress/$goalTotal * 100);
    
    if($goalPercent > 100) {
        $goalPercent    = 100;
    }
    
    $goalRemaining  = dw_campaigns_force_decimal($goalTotal - $goalProgress, $campaign->field_dw_currency['0']['value']);
    $goalTotal      = dw_campaigns_force_decimal($goalTotal);
    $goalProgress   = dw_campaigns_force_decimal($goalProgress);

    if($goalRemaining < 0) {
        $goalRemaining		= '0.00';
    }

/*
    echo "Goal Progress: " . $goalProgress . "<br>";
    echo "Goal Total: " . $goalTotal . "<br>";
    echo "Goal Remaining: " . $goalRemaining . "<br>";
    echo "Goal Percent: " . $goalPercent . "<br>";
*/
?>
    <div class="left">
        <span class="label"><?php echo t('Raised so far...'); ?></span>
        <div class="progress-bar progress-blue"> <div class="progress-inner"><span><?php echo $goalProgress;?></span></div></div>
    </div>
    <div class="right">
        <span class="label"><?php echo t('toward goal of'); ?></span>
        <div class="progress-bar progress-yellow"><div class="progress-inner"><span><?php echo $goalTotal;?></span></div></div>
    </div>
    <div class="progress-bar progress-yellow progress-pcp"><div class="progress-inner" style="width:<?php echo $goalPercent;?>%"><span><?php echo $goalPercent;?>%</span></div></div>
