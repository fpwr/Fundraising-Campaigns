<?php
    global $language;

    $about_link = $language->language == 'en' ? '/content/about-us' : '/node/86';
?>
<div class="where-the-funds-go">
    <h2><?php echo t('Where the Funds Go'); ?></h2>
<p>
<?php echo t('All proceeds from the One SMALL Step walks go to fund the Prader-Willi Syndrome Research plan jointly created by FPWR and PWSAUSA.'); ?>
<?php echo t('Click on the buttons below to find more information on the research plan and how the organizations are working together.'); ?>
</p>
    <a href="http://www.fpwr.org/sites/default/files/imagefield_default_images/PWSResearchPlan_rev2011.pdf" class="btn btn-light-blue"><?php echo t('About the Prader-Willi Syndrome Research Plan'); ?></a>
    <a href="<?php echo $about_link; ?>" class="btn btn-dark-blue"><?php echo t('FPWR / PWSA(USA) Working Together'); ?></a>
</div>
