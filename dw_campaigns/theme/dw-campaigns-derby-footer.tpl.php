<?php
if(dw_campaigns_get_selected_type()!='derby')
    return;
?>
<div class="derby-footer">
    <ul>
        <li>
        <?php echo t('(c)  2011 PWSACO Benefitting Prader-Willi Syndrome Association of Colorado'); ?>
        </li>
        <li class="contact-us">
            <a href="#"><?php echo t('Contact Us'); ?></a>
        </li>
        <li>
            <a href="http://facebook.com"><img src="/sites/all/themes/dw_campaigns_derby/images/footerFacebook.jpg"><span><?php echo t('Connect with us on Facebook'); ?></span></a>
        </li>
        <li>
            <a href="http://twitter.com"><img src="/sites/all/themes/dw_campaigns_derby/images/footerTwitter.jpg"><span><?php echo t('Follow us on Twitter'); ?></span></a>
        </li>
    </ul>
</div>
