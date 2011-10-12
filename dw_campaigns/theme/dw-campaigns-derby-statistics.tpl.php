<?php
    // this filename is a bit wrong, this is also used for walking for the sideways thermometer
    
    $type           = dw_campaigns_get_selected_type();
  
    // in walking we want to get all campaigns if we don't have a specific campaign  
    if($type == 'walking') {
        $terse      = TRUE;
        
        if(!is_null($campaign)) {
            $res        = _dw_campaigns_campaign_total($campaign);   
        } else {
            $res        = _dw_campaigns_campaigns_total();
        }
    } else {
        $res            = _dw_campaigns_campaign_total($campaign);
    }

    $currency = isset($campaign->field_dw_currency['0']['value']) ? $campaign->field_dw_currency['0']['value'] : 'USD';
    
    $goalTotal      = $res['goal'];

    if(arg(2) == 'current') {
        $goalAlt        = variable_get('dw_campaigns_fundraising_goal_override', '0');
        if($goalAlt > 0) {
           $goalTotal   = $goalAlt;
        }
    }

    $goalProgress   = $res['raised'];

    // we only want to convert everything to USD if we are not looking at a specific campaign, specific campaigns should be shown in the campaigns currency
    if(isset($res['raised_usd']) && is_null($campaign)) { 
	$goalProgress   = $res['raised_usd'];
    }

    if(is_null($campaign)) {
        $goalExtra      = variable_get('dw_campaigns_fundraising_goal_start_value', '0');
        $goalProgress   += $goalExtra;
    }

    if($goalTotal == 0) {
        drupal_set_message(t('Unable to load campaign goals'), 'error');
        return;
    }
    // I assume we want to round 99.9 down so that we don't say 100% too soon
    $goalPercent                    = floor($goalProgress/$goalTotal * 100);
	if($goalPercent > 100) {
		$goalPercent = 100;
	}

    $goalRemaining                  = dw_campaigns_force_decimal($goalTotal - $goalProgress, $currency);
    $goalTotal                      = dw_campaigns_force_decimal($goalTotal, $currency);
    $goalProgress                   = dw_campaigns_force_decimal($goalProgress, $currency);
    
    if($goalRemaining < 0) {
        $goalRemaining              = 0;
    }
    
    $daysEnd                        = strtotime($campaign->field_dw_date_range[0]['value2']);
    $timeNow                        = time();

    if($timeNow>$daysEnd) {
        $daysLeft                   = -1;
    } else {
        $daysLeft                   = ceil(($daysEnd - time()) / (3600*24));
    }

    //echo "Goal Progress: " . $goalProgress . "<br>";
    //echo "Goal Total: " . $goalTotal . "<br>";
    //echo "Goal Remaining: " . $goalRemaining . "<br>";
    //echo "Goal Percent: " . $goalPercent . "<br>";
    //if(!$terse) {
        //echo "Days Left: " . $daysLeft . " (will show -1 if its passed)<br>";
    //}
?>
<div class="thermoEmpty">
	<div class="thermoFull" style="width: <?php echo $goalPercent; ?>%">
		<div class="thermoHorse">
			<?php echo $goalPercent ?>%
		</div>
	</div>
</div>


<ul class="stats">
	<li>
		<span class="dollar-label"><?php echo t('Raised:'); ?></span>
		<span class="dollar-amount"><?php echo $goalProgress; ?></span>
	</li>
	<li>
		<span class="dollar-label"><?php echo t('Our Goal:'); ?></span>
		<span class="dollar-amount"><?php echo $goalTotal; ?></span>
	</li>
</ul>
<?php if(!$terse) { ?>
<div class="remaining">
	<span class="days"><?php echo t('!days Days !closespan left until finish', array('!days' => $daysLeft, '!closespan' => '</span>')); ?>
</div>
<?php } ?>
