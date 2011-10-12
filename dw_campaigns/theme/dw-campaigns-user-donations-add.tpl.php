<?php
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
