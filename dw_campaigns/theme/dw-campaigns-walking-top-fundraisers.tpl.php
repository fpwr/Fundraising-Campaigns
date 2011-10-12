<?php
    if(dw_campaigns_get_selected_type()!='walking')
        return;

    global $location_string;
    global $selected;

    $campaign = dw_campaigns_get_selected_campaign();

    $campaign_id = NULL;

    if(!is_null($campaign)) {
        $leaders        = _dw_campaigns_campaign_leaders($campaign, $show_cnt, TRUE);
        $campaign_id    = $campaign->nid;
    } else {
        $leaders        = _dw_campaigns_all_leaders($show_cnt);
    }
    

?>

<h2><?php echo t('Top Fundraisers'); ?></h2>
<?php
    if($show_cnt<10) {
        $leaderboard_path       = '/dw/campaign/current/leaderboard';
        
        if(!is_null($campaign_id)) {
            $leaderboard_path   = dw_campaigns_get_campaign_path($campaign_id, '/dw/campaign', 'leaderboard');
        }
?>
        <a href="<?php echo $leaderboard_path;?>" class="see-all"><?php echo t('see all'); ?></a>
<?php
    }

    if(!empty($location_string) && $selected != -1) {
        echo '<span class="for-locname">' . t('For !city', array('!city' =>  $location_string)) . '</span>';
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
            $our_campaign = $campaigns[$leader['campaign_id']];
?>
    <li>
        <div class="left"><a style="background-image:url('<?php echo $leader['image']; ?>');" href="<?php echo $leader['url'];?>"></a></div>
		<div class="right">
			<a href="<?php echo $leader['url'];?>" class="dollar-label"><?php echo $leader['name'];?></a>
			<a href="<?php echo $leader['url'];?>" class="dollar-amount"><?php echo dw_campaigns_force_decimal($leader['total'], $our_campaign->field_dw_currency['0']['value']);?></a>
<?php
if(arg(2)!='location') {
?>
			<a href="/dw/walking/location/<?php echo $leader['campaign_id'];?>" class="location-name"><?php echo $leader['campaign_location'];?></a>
<?php
}
?>
        </div>
    </li> 
<?php
        }
    } else {
        echo '<li><div class="left"></div><div class="right">' . t('No Fundraisers Yet :(') . '</div></li>';
    }
?>
    </ul>


</div>
