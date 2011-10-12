<?php
     $image_params   = array(
        'w'                 => 100,
    );
    $matched_image  = '';
    //get the user and drop it in for a preview
    $imageSrc       = _dw_campaigns_get_photo($thisUser,  $image_params, 'pcp-photo', $pcp->id, $matched_image);

    if(arg(2) == 'give' && arg(3) == 'general-donation') {
        echo '<div class="blurb">To make a donation to an individuals fundraising page, please return to the home page and select the location in which the individual is participating.</div>';
    }
?>
    <h2><?php echo t('Donate to @title', array('@title' => $pcp->title));?></h2>
    <img src="<?php echo $imageSrc;?>">

<?php
    $form   = drupal_get_form('dw_campaigns_users_donate_page_form', $campaign, $pcp);
    echo $form; 
?>
<div style="display:none">
	<a href="#hidden-words" id="show-words"></a>
	<div id="hidden-words">
		<span class="please-wait"><?php echo t('Please wait, we are processing your donation.'); ?></span>
		<br>
		<span class="please-wait-extra"><?php echo t('Reloading or navigating away from this page may cause multiple donations'); ?></span>
	</div>
</div>
