<h2>
    <span class="register-account"><?php echo t('Register for an account');?></span>
<?php
    $mode_type = dw_campaigns_get_selected_type();
    if($mode_type == 'walking') {
        echo t(' - Step 1 of 2');
    }
?>
</h2>
<?php echo $registerForm; ?>
