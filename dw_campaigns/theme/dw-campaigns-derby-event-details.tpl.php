<?php
    if(arg(0)!='dw') {
        return;
    }
?>
<script type="text/javascript">
$(document).ready(function() { 
    $('.rot13').each(undoRot);

    function undoRot(n) { 
        var letl = "abcdefghijklmnopqrstuvwxyz";
        var letc = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	var temp = $(this).attr('href');
        var len  = temp.length;
        var onec = "";
        var res  = "";
        var out  = "";
        var pos  = "";

        for(i = 0; i < len ; i++) {
            onec = temp.substr(i, 1);
            isup = letc.indexOf(onec);
            if(isup != -1) {
                onec = letl.substr(isup, 1);
            }
            res = letl.indexOf(onec);
            if (res != -1) {
                pos = (res + 13) % 26;
                if(isup == -1) {
                    out = out + letl.substr(pos, 1);
                } else {
                    out = out + letc.substr(pos, 1);
                }
            } else {
                out = out + onec;
            }
        }
        $(this).attr('href', out);
    };
});
</script>
<?php

    $node       = dw_campaigns_get_selected_campaign();
    $mode_type	= dw_campaigns_get_selected_type();
	
    if($node->nid <= 0) {
        return;
    }
    $url        = NULL;
    $event_id   = dw_campaigns_get_event_page_id_from_campaign($node);

    if(is_numeric($event_id) && $event_id > 0) {
        $baseurl    = variable_get('dw_campaigns_cfg_base_url', '');
        if(!empty($baseurl)) {
            $url    = sprintf("%s/civicrm/event/info?id=%s&reset=1", $baseurl, $event_id);
            $url    = sprintf("%s/civicrm/event/register?id=%s&reset=1", $baseurl, $event_id);
        }
    }

    $shortened      = FALSE;

    $details = $node->teaser;
    if(strlen($details)>150) {
        $details    = substr($details, 0, 150) . '...';
	$shortened  = TRUE;
    }

    $contact_data   = '';
    $contact_phone  = '';

    $extended = NULL;

    if($mode_type == 'walking') {
        $contact_phone = $node->field_dw_contact_phone[0]['value']; 
        $contact_name  = $node->field_dw_contact_name[0]['value']; 
        $contact_email = $node->field_dw_contact_email[0]['value']; 

        if(!empty($contact_name)) {
 
	    $contact_data = $contact_name;
	
       	    if(!empty($contact_email)) { 
	        $contact_m     = str_rot13("mailto:$contact_email");
	        $contact_data = '<a href="' . $contact_m . '" class="rot13">' . $contact_name . '</a>';
	    }
        }

        $result     = db_query("select * from content_type_dw_campaigns_event_page where field_dw_eventdetails_node_value = '%s'", $node->nid);
	$extended   = db_fetch_object($result);        
        if(!is_null($extended)) {
            $extended_data = node_load($extended->nid);
        }
    }

?>
<div class="derby-event-details">
    <table>
        <tr class="date">
            <th><?php echo t('Date'); ?></th><td><?php echo $node->field_dw_event_date[0]['value'];?> <?php echo $node->field_dw_event_time[0]['value']; ?></td>
        </tr>
        <tr class="location">
            <th><?php echo t('Location'); ?></th><td><?php echo $node->field_dw_event_location[0]['value']; ?></td>
        </tr>
        <tr class="details">
            <th><?php echo t('Details'); ?></th><td><?php echo $details; ?>
        <?php
            if(!is_null($url) && $mode_type != 'walking') {
        ?>
            <br><a href="<?php echo $url;?>" target="_blank"><?php echo t('Register for Event'); ?></a>
	<?php
            }
	?>
        <?php
           // we also show this for walking...
           if(!is_null($extended) || $shortened || $mode_type == 'walking') {
        ?>
            <br><a class="fb" href="/dw/walking/event-extended/<?php echo $node->nid;?>?ajax=1">Get more details</a>
        <?php 
           }
        ?>
            </td>
        </tr>
<?php
        if($mode_type == 'walking' && !empty($contact_data)) {
?>
        <tr class="contact">
            <th><?php echo t('Contact'); ?></th>
            <td><?php echo $contact_data;?>
                <?php if(!empty($contact_phone)) { 
                          echo "$contact_phone"; 
                      }
                ?></td>
        </tr>
<?php      
        }
        if(!is_null($extended_data->field_dw_eventdetails_dl_documen[0]['filepath'])) {
?>
        <tr class="document">
            <th><?php echo t('Document'); ?></th>
            <td><a href="/<?php echo $extended_data->field_dw_eventdetails_dl_documen[0]['filepath']; ?>">Download</a></td>
        </tr>

<?php
        }
?>
    </table>
</div>
