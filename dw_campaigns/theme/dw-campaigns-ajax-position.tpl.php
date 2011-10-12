<?php
    // remove unnecessary characters
    $possible               = preg_replace('/[^0-9\.]/', '', $possible);
    
    $contributionPage       = new stdClass;
    $contributionPage->id   = $pcp->contribution_page_id;
    $campaign               = dw_campaigns_get_campaign_from_contribution_page($contributionPage);

    // we get $thisUser, $campaign, $pcp

    $res = dw_campaigns_get_contribution_total_for_pcp($pcp);
    $pcpTotal     = $res['total'];

    $goalTotal    = $pcp->goal_amount;
    $goalProgress = $pcpTotal + $possible;
    $goalProgress_nofmt = $goalProgress;

    // I assume we want to round 99.9 down so that we don't say 100% too soon
    $goalPercent	= floor($goalProgress/$goalTotal * 100);
    
    if($goalPercent > 100) {
        $goalPercent    = 100;
    }
    
    $goalRemaining	= $goalTotal - $goalProgress;

    if($goalRemaining < 0) {
        $goalRemaining		= '0.00';
    }
    
    $res            = _dw_campaigns_campaign_find_position_of_amount($campaign, $goalProgress);
    // sets $postion and $total
    extract($res);  

    $position_name  = position_to_name($position);
    
    $results        = _dw_campaigns_campaign_leaders_list_fake($campaign, $pcp->id, $goalProgress);
    $pos            = $results['totals'];

    $goalTotal      = dw_campaigns_force_decimal($goalTotal, $campaign->field_dw_currency['0']['value']);
    $goalProgress   = dw_campaigns_force_decimal($goalProgress, $campaign->field_dw_currency['0']['value']);
    $goalRemaining  = dw_campaigns_force_decimal($goalRemaining, $campaign->field_dw_currency['0']['value']);    
?>

<div class="live-donation">

<div class="how-we-will-do">
    <h2>After your donation</h2>
    <span class="words"><?php echo $thisUser->displayname; ?> will sit in</span>
    <div class="yellow-box">
        <div class="left">
    <?php
        if($position_name != 'N/A') {
            echo '<span class="position-number">' . $position_name . '</span> <span class="position-place">Place</span>';
        } else {
            echo "Not Ranked Yet";
        }
    ?>
        </div>
        <div class="right">
            <span class="with">with</span><span class="money"><?php echo $goalProgress;?></span><span class="donations">in donations</span>
        </div>
    </div>

<!--
    <span class="intro">Your donation will update the racer's progress to</span>
    <span class="progress"><?php echo $goalProgress;?></span>
    <span class="toward">toward a goal of</span>
    <span class="total"><?php echo $goalTotal;?></span>
    <div class="progress-bar progress-yellow progress-pcp"><div class="progress-inner" style="width:<?php echo $goalPercent;?>%"><span><?php echo $goalPercent;?>%</span></div></div>
    <span class="outro">and put them in</span>
    <span class="position-number"><?php echo $position_name;?></span>
    <span class="position-place">Place</span>
-->
    <div class="updated-leaderboard">
        <h2>Updated Leaderboard</h2>
            <table>
        <?php
            $i = 0;
            foreach($pos as $contact_id => $total) {
                $i++;
                $drupal_id      = _dw_campaigns_contact_id_get_user($contact_id);
                $fake_user      = user_load(array('uid'=>$drupal_id));
                $url            = dw_campaigns_user_get_pcp_url($fake_user, $campaign);
        
                $image_match    = '';
                $image_params   = array(
                    'w'                 => 100,
                    'contribution'      => true,
                );
                
                $photo          = _dw_campaigns_get_photo($fake_user, $image_params, 'user-photo', NULL, $image_match);
    
        
				$class = ($contact_id == $pcp->contact_id) ? 'me' : '';
        ?>
                <tr class="<?php echo $class; ?>">
                    <td class="name">
                        <img src="<?php echo $photo; ?>" width="25"> <a href="<?php echo $url; ?>"> <?php echo $fake_user->displayname;?></a>
                    </td>
                    <?php
                    if($contact_id == $pcp->contact_id) {
                    ?>
	                    <td class="leader-diff">
	                    	<div class="leader-total"><?php echo $goalProgress; ?></div>
                    	</td>
                    <?php
                    } else {
                        $diff   = $total - $goalProgress_nofmt;
                        if($diff < 0) {
                            $diff_sentence = dw_campaigns_force_decimal(abs($diff), $campaign->field_dw_currency['0']['value']) . " behind";
                        } else {
                            $diff_sentence = dw_campaigns_force_decimal(abs($diff), $campaign->field_dw_currency['0']['value']) . " ahead";
                        }
                    ?>
                    <td class="leader-diff">
                    	<div class="leader-total"><?php echo dw_campaigns_force_decimal($total, $campaign->field_dw_currency['0']['value']); ?></div>
                    	<div class="diff"><?php echo $diff_sentence; ?></div>
                	</td>
                    
                    <?php
                    }
                    ?>
                </tr>
        <?php
                if($i==3) { break; }
            }
        ?>
            </table>
    </div>
</div>
