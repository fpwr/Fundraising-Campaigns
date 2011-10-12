<?php
if(dw_campaigns_get_selected_type()!='walking')
    return;
?>

<div class="walking-footer">
    <ul>
        <li class="copyright">
        <?php echo t('(c)  2011 Foundation for Prader-Willi Research.  All Rights Reserved'); ?>
        </li>
        <li class="contact-us">
            <a href="/node/61"><?php echo t('Contact Us'); ?></a>
        </li>
        <li class="facebook">
            <a href="http://www.facebook.com/pages/The-Foundation-for-Prader-Willi-Research/78626677947"><img src="/sites/all/themes/dw_campaigns_walking/images/footerFacebook.jpg"><span><?php echo t('Connect with us on Facebook'); ?></span></a>
        </li>
        <li class="twitter">
            <a href="http://twitter.com/#!/fpwr"><img src="/sites/all/themes/dw_campaigns_walking/images/footerTwitter.jpg"><span><?php echo t('Follow us on Twitter'); ?></span></a>
        </li>
    </ul>
</div>
