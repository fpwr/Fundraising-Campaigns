<?php
    $campaign_id = dw_campaigns_get_default_campaign();
    
    if(!is_null($campaign)) {
        $leaders        = _dw_campaigns_campaign_leaders($campaign, $show_cnt);
        $campaign_id    = $campaign->nid;
        $our_campaign   = $campaign;
    } else {
        $leaders = _dw_campaigns_all_leaders($show_cnt);
    }

?>

<div class="fundraising-leaders">
    <ul>
<?php
    if($leaders) {
        $campaigns = array();
        foreach($leaders as $leader)
        {
            if(!isset($campaigns[$leader['campaign_id']])) {
                $campaigns[$leader['campaign_id']] = node_load($leader['campaign_id']);
            }
            $our_campaign       = $campaigns[$leader['campaign_id']];
?>
    <li>
        <div class="left"><a style="background-image:url('<?php echo $leader['image']; ?>');" href="<?php echo $leader['url'];?>"></a></div>
		<div class="right">
			<a href="<?php echo $leader['url'];?>" class="dollar-label"><?php echo $leader['name'];?></a>
			<a href="<?php echo $leader['url'];?>" class="dollar-amount"><?php echo dw_campaigns_force_decimal($leader['total'], $our_campaign->field_dw_currency['0']['value']);?></a>
		</div>
    </li>
<?php
        }
    }
?>
    </ul>
<?php
    if($show_cnt<10) {
?>
        <a href="<?php echo dw_campaigns_get_campaign_path($campaign_id, '/dw/campaign', 'leaderboard');?>" class="see-all"><?php echo t('see all'); ?></a>
<?php
    }
?>
</div>
