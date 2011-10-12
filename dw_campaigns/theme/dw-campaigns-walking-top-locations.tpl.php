<?php
    global $selected;

    $res = _dw_campaigns_campaigns_campaign_total();
    extract($res); // gives $totals, $usd_totals and $campaigns
  
   // if its not a lot, we assume its for the bottom of the homepage 
   if($show_cnt < 100) {
?>    

<div class="top-locations">
    <h2><?php echo t('Top Locations'); ?></h2>
    <?php
        if($show_cnt<10) {
    ?>
    <a href="/dw/walking/toplocations" class="see-all"><?php echo t('See All'); ?></a>
    <?php
        }
    ?>
    <ul>
        <?php
            if(!isset($selected)) {
                $selected = NULL;
            }
            
            $i  = 0;
            foreach($usd_totals as $nid => $usd_total) {
                if(dw_campaigns_hide_campaign($campaigns[$nid])) {
                    continue;
                } else {
                    $our_campaign = $campaigns[$nid];
                    $i++;
        ?>
            <li<?php if($nid==$selected) echo ' class="location-selected"'; ?>>
                <div class="left"><a href="/dw/walking/location/<?php echo $nid; ?>" class="location-label"><?php echo $campaigns[$nid]->field_dw_campaign_location[0]['value']; ?></a></div>
                <div class="right"><a href="/dw/walking/location/<?php echo $nid; ?>" class="dollar-amount"><?php echo dw_campaigns_force_decimal($totals[$nid], $our_campaign->field_dw_currency['0']['value']); ?></a></div>
            </li>
        <?php
                    if($i == $show_cnt) {
                        break;
                    }
                }
            }
        ?>
    </ul>
</div>
<?php
    } else {

    $num_per_page = $show_cnt; // 999999

    $headers        = array(
        array(
            'data'  => t('Position'),
            'field' => 'position',
        ),
        array(
            'data'  => t('Location'),
            'field' => 'location',
        ),
        array(
            'data'  => t('Fundraisers'),
            'field' => 'fundraisers',
        ),
        array(
            'data'  => t('Total Raised'),
            'field' => 'amount',
            'sort'  => 'desc'
        )
    );


    $query = "
        CREATE TEMPORARY TABLE
            leader_as_$num_per_page
            (
                name char(255),
                photo char(255),
                position int,
                amount float,
                raw_amount float,
                location char(255),
                fundraisers int,
                url char(255),
                campaign_id int
            )
    ";
    db_query($query);

    $position   = 0;
    $rows       = array();

    foreach($usd_totals as $nid => $usd_total) {
        $campaign       = $campaigns[$nid];

        if(dw_campaigns_hide_campaign($campaign)) {
           continue;
        }


        $position++;

        $image_match    = '';
        $image_params   = array(
            'w'                 => 100,
            'contribution'      => true,
        );

        $name           = $campaign->title;
        $photo_file     = 'sites/all/themes/dw_campaigns_walking/images/no-image.gif';

        $temp_filename = $campaign->field_dw_campaign_image[0]['filepath'];
        if(file_exists($temp_filename)) {
            $photo_file = $temp_filename;
        }

        $photo  = _dw_campaigns_thumb($photo_file, $image_params);

        $amount         = dw_campaigns_convert_to_usd($campaign->field_dw_currency['0']['value'], $totals[$nid]);
        $raw_amount     = $totals[$nid];
        $location       = $campaign->field_dw_campaign_location[0]['value'];
        $fundraisers    = $pcp_counts[$nid];
        $url            = '/dw/walking/location/' . $nid;
        
        
        db_query("insert into {leader_as_$num_per_page} (name, photo, position, amount, raw_amount, location, fundraisers, url, campaign_id) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", $name, $photo, $position, $amount, $raw_amount, $location, $fundraisers, $url, $campaign->nid);
    }

    $sql_count = "select count(*) from leader_as_$num_per_page";

    //$result = db_query("select * from donations_as " . tablesort_sql($headers));
    $result = pager_query("select * from leader_as_$num_per_page " . tablesort_sql($headers), $num_per_page, 0, $sql_count);

    $rows = array();
    while ($db_row = db_fetch_object($result)) {
        $our_campaign = $campaigns[$db_row->campaign_id];

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->position, 'class' => 'position'),
                array('data' => '<a href="' . $db_row->url . '"> <img src="' . $db_row->photo . '" width="50"> <span class="location">' . $db_row->location . '</span></a>', 'class' => 'photo' ),
                array('data' => $db_row->fundraisers, 'class' => 'fundraisers'),
                array('data' => dw_campaigns_force_decimal($db_row->raw_amount, $our_campaign->field_dw_currency['0']['value']), 'class' => 'amount'),
            )    
        );
    }

    echo "<h2>" . t('One Small Step Locations') . "</h2>";
    echo theme('table', $headers, $rows);
    echo theme('pager', NULL, $num_per_page, 0);

   }
?>
