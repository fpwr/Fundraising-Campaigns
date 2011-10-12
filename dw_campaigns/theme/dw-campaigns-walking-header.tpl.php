<?php
    if(dw_campaigns_get_selected_type()!='walking')
        return;
?>

<link rel="stylesheet" type="text/css" href="/modules/dw_campaigns/fireworks/style/fireworks.css" media="screen" />
<div id="fireworks-template">
 <div id="fw" class="firework"></div>
 <div id="fp" class="fireworkParticle"><img src="/modules/dw_campaigns/fireworks/image/particles.gif" alt="" /></div>
</div>

<div id="fireContainer"></div>
<script type="text/javascript">
$(document).ready(function() {
    var do_nav = function(cur_val) {
        if(cur_val != '') {
            if($("body").hasClass('leaderboard')) { 
                window.location.href = "/dw/walking/location/" + cur_val + '/go';
            } else {
                window.location.href = "/dw/walking/location/" + cur_val;
            }
        }
        return false;
    };

    $(".walking-header-bottom .jqTransformSelectWrapper ul li a").click(function(){
        var cur_val = $("#walk_location").val();
	do_nav(cur_val);
    });
    
    $(".walking-header-bottom #walk_location").change(function(){
        var cur_val = $("#walk_location").val();
	do_nav(cur_val);
    });

    $(".support-right .jqTransformSelectWrapper ul li a").click(function(){
        var cur_val = $("#walk_location2").val();
	do_nav(cur_val);
    });

});

</script>

<?php

    global $options;
    global $selected; // lets share this with other blocks
    global $location_string;

    $no_location = FALSE;
    
    if(arg(1) == 'user') {
        $no_location = TRUE;
    }   
 
    $locations = dw_campaigns_get_all_campaign_location();
    $location_string = '';

    $doMenu   = FALSE;
    
    if(strstr(dw_campaigns_make_body_class(), 'walking-location') !== FALSE) {
        $selected   = dw_campaigns_get_selected_location();
        $doMenu     = TRUE;
    }
   
    if(arg(1) == 'users' || arg(3) == 'leaderboard' || arg(3) == 'search') {

        $campaign  = dw_campaigns_get_selected_campaign();
        if(!is_null($campaign)) {
            $selected = $campaign->nid;
        }
    }

    if($doMenu) {
        if(is_null($selected)) {
            $selected = -1;
        } else {
            if(isset($locations[$selected])) {
                $campaign = node_load($selected);
                dw_campaigns_set_selected_campaign($campaign);
                $location_string    = $campaign->field_dw_campaign_location[0]['value'];
            } else {
                if($selected != 0) {
                    echo "invalid node ($selected)";
                }	
                dw_campaigns_set_selected_campaign(NULL);
            }
        }
    }

    $options    = '';
    foreach($locations as $nid => $location) {
       $options .= sprintf('<option value="%d" %s>%s</option>', $nid, ($nid==$selected)?'selected=selected':'', $location);
    }

?>
<div class="walking-header-left">
<div class="nav-logos">
	<a href="/"><div class="header-main-logo"></div></a>
	<a href="http://www.fpwr.org"><div class="header-second-logo"></div></a>
	<a href="http://www.pwsausa.org""><div class="header-third-logo"></div></a>
</div>
<div class="walking-header-right">
    <div class="account-box">
    <?php
        if($user->uid>0) {
    ?>
        <div class="signed-in">Signed in as <?php echo $user->name;?> 
    <?php
            $res = dw_campaigns_get_user_pcp_details($user);
            if(!empty($res['url'])) {
                echo '<a href="' . $res['url'] . '" class="goto-page">Go to my page</a>';
            }
    ?>
        </div><a href="/logout?destination=dw" class="btn"><?php echo t('Logout'); ?></a>
    <?php
        } else {
            echo l(t('Login'),"dw/user/login");  		
        }
    ?>
    </div>
</div>
<div class="walking-header-bottom">
    <?php
        if(!empty($location_string)) {
    ?>    
    <span class="locname"><a href="/dw/walking/location/<?php echo $selected; ?>"><?php echo $location_string; ?></a></span>
    <?php
        }
    ?>
<div class="find-a-walk-word"><?php echo t('Find A Walk'); ?></div>
<?php
if(!$no_location) {
?>
    <form method="get" action="/dw/walking/">
        <select name="city" id="walk_location">
            <option value="0"><?php echo t('All Locations'); ?></option>
            <?php echo $options; ?>
        </select>
    </form>
<?php
}
?>
    <a href="/dw/walking/distance-search" class="find-a-walk"><?php echo t('Find a Walk Near Me'); ?></a>
</div>
<div class="languages" style="float:right;">

<?php
    $path = drupal_is_front_page() ? '<front>' : $_GET['q'];
    $languages = language_list('enabled');
    $links = array();
    foreach ($languages[1] as $language) {
        $flag_image = base_path() . path_to_theme() .'/images/flags/'.$language->language.'.gif';

        $links[$language->language] = array(
            'href'       => 'dw/lang/' . $language->language . '/' . $path,
            'title'      => '<img src="' . $flag_image . '"> ' . $language->native,
            'language'   => $language,
            'html'       => TRUE,
            'attributes' => array(
                'id'       => $language->language,
                'lang'     => $language->language,
                'title'    => t('Watch this page in @language.', array('@language' => $language->native), $language->language),
            ),
        );
    }

    drupal_alter('translation_link', $links, $path);

    if(function_exists('i18n_get_lang')) {
        $current_language = i18n_get_lang();
        unset($links[$current_language]);
    }


    echo theme('links', $links, array());

?>
</div>
