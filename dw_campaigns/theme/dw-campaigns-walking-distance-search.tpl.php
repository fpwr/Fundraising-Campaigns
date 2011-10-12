<?php
    $local = dw_campaigns_get_visitor_location();
    if($local['known']) {
        $search_terms = $local['city'] . ', ' . $local['region'];
    } else {
        $search_terms = "United States";
    }
    
    if(isset($_REQUEST['query'])) {
        $search_terms = $_REQUEST['query'];
    }
    
    $campaigns = dw_campaigns_get_close_campaigns();
    
    $rows = '';
    $units = variable_get('dw_campaigns_walking_distance_unit', 'M');
    switch($units) {
        case 'M':
            $units_wd = t('Miles');
            break;
        case 'K':
            $units_wd = t('Kilometers');
            break;
        default:
            $units_wd = '';
            break;
    }

    $num_per_page = 40;

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
            'data'  => t('Distance'),
            'field' => 'distance',
            'sort'  => 'asc'
        ),
        array(
            'data'  => t('Number of Walkers'),
            'field' => 'walkers',
        ),
        array(
            'data'  => t('Total Raised'),
            'field' => 'amount',
        )
    );


    $query = "
        CREATE TEMPORARY TABLE
            distance_as_$num_per_page
            (
                position int,
                location char(255),
                distance float,
                walkers int,
                amount float,
                campaign_id int
            )
    ";
    db_query($query);

    $position   = 0;
    $rows       = array();
    foreach($campaigns as $id => $campaign) {
        $pcps                = _dw_campaigns_get_pcps_for_campaign($campaign);
        
        $position++;
        $location           = $campaign->field_dw_campaign_location[0]['value']; // $campaign->title;
        list($distance,)    = split('-', $id);
        $walkers            = count(get_object_vars($pcps));
        $amount             = dw_campaigns_get_contribution_total_for_campaign($campaign);
        $campaign_id        = $campaign->nid;
        
        db_query("insert into {distance_as_$num_per_page} (position, location, distance, walkers, amount, campaign_id) VALUES ('%s', '%s', '%s', '%s', '%s', '%s')", $position, $location, $distance, $walkers, $amount, $campaign_id);

    }


    $sql_count = "select count(*) from distance_as_$num_per_page";

    $result = pager_query("select * from distance_as_$num_per_page " . tablesort_sql($headers), $num_per_page, 0, $sql_count);

    $rows = array();
    while ($db_row = db_fetch_object($result)) {
        $our_campaign = node_load($db_row->campaign_id);

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->position, 'class' => 'position'),
                array('data' => '<a href="/dw/walking/location/' . $db_row->campaign_id . '">' . $db_row->location . '</a>', 'class' => 'location'),
                array('data' => floor($db_row->distance) . ' ' . $units_wd, 'class' => 'distance'),
                array('data' => $db_row->walkers,  'class' => 'walkers'),
                array('data' => dw_campaigns_force_decimal($db_row->amount, $campaign->field_dw_currency['0']['value'])),
            )    
        );
    }
    
?>

<form method="post">
    <p><?php echo t('Enter a Zip code, or a City, State below'); ?></p> 
    <?php echo t('Searching From '); ?><input type="text" name="query" class="location-search" value="<? echo htmlentities($search_terms);?>">
    <input type="submit" value="<?php echo t('Find Distance'); ?>">
</form>

<?php
    echo theme('table', $headers, $rows);
    echo theme('pager', NULL, $num_per_page, 0);
?>
