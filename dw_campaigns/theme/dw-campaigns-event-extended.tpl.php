<?php
    if(is_null($campaign)) {
        return;
    }

    $node       = $campaign;
    $node       = (is_object($node)) ? $node : node_load($node);

    $result     = db_query("select * from content_type_dw_campaigns_event_page where field_dw_eventdetails_node_value = '%s'", $node->nid);
    $extended   = db_fetch_object($result);
    if(!is_null($extended)) {
        $extended_data = node_load($extended->nid);
    }

    $details = $node->teaser;


    include_once('dw-campaigns-map.tpl.php');
?>
    <div id="extended" class="derby-event-details">
        <h2>Event Details</h2>
        <table>
            <tr class="details">
                <th><?php echo t('Details'); ?></th><td><?php echo $details; ?>
            </tr>
<?php
        if(!is_null($extended_data->body)) {
?>
            <tr class="details">
                <th><?php echo t('More Details'); ?></th><td><?php echo $extended_data->body; ?>
            </tr>
<?php
        }
?>
<?php 
        if(!is_null($extended_data->field_dw_eventdetails_prizes[0]['value'])) {
?>
            <tr class="prizes">
            <th><?php echo t('Prizes'); ?></th>
            <td><?php echo $extended_data->field_dw_eventdetails_prizes[0]['value']; ?></td>
            </tr>
<?php 
        }
?>
            <tr class="map">
                <th><?php echo t('Map'); ?></th>
                <td> <?php echo $event_map; ?> </td>
            </tr>
        </table>
    </div>
