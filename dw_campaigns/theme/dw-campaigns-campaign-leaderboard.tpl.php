<?php

function sort_leaders($a, $b) {

    if($a['amount'] < $b['amount']) {
       return 1;
    }

    if($a['amount'] > $b['amount']) {
       return -1;
    }
    
    return 0;

}

    if(!is_null($campaign)) {
        $leaders = _dw_campaigns_campaign_leaders($campaign, 9999);
    } else {
        $leaders = _dw_campaigns_all_leaders(9999);
    }
    
    $type = dw_campaigns_get_selected_type();

    $num_per_page = 40;

    $headers        = array(
        array(
            'data'  => t('Position'),
            'field' => 'position',
        ),
        array(
            'data'  => t('Name'),
            'field' => 'name',
        ),
        array(
            'data'  => t('Location'),
            'field' => 'location',
        ),
        array(
            'data'  => t('Donations'),
            'field' => 'donations',
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
                campaign_id int,
                location char(255),
                donations int,
                url char(255)
            )
    ";
    db_query($query);

    $position   = 0;
    $rows       = array();
    $campaigns  = array();
    if(count($leaders) > 0) { 
        $sorted_leaders = array();

        foreach($leaders as $leader) {
            if(!isset($campaigns[$leader['campaign_id']])) {
                $campaigns[$leader['campaign_id']] = node_load($leader['campaign_id']);
            }
            $our_campaign       = $campaigns[$leader['campaign_id']];
            $amount             = dw_campaigns_convert_to_usd($our_campaign->field_dw_currency['0']['value'], $leader['total']);
            $leader['amount']   = $amount;
            $sorted_leaders[]   = $leader;
        }

        usort($sorted_leaders, "sort_leaders");

        foreach($sorted_leaders as $leader) {

            $campaign_id    = $leader['campaign_id'];

            $position++;
            $image_match    = '';
            $image_params   = array(
                'w'                 => 100,
                'contribution'      => true,
            );
            $fake_user      = user_load(array('uid' => $leader['drupal_id']));
            $contact        = $leader['contact'];
    
            $name           = $leader['name'];
            $photo          = _dw_campaigns_get_photo($fake_user, $image_params, 'user-photo', NULL, $image_match);
            $raw_amount     = $leader['total'];
            $amount         = $leader['amount'];   
 
            if($type != 'walking') {
                $location   = $contact->city . ', ' . $contact->state_province;
            } else {
                $location   = $leader['campaign_location'];
            }
            
            $donations      = $leader['donations'];
            $url            = $leader['url'];
            
            
            db_query("insert into {leader_as_$num_per_page} (name, photo, position, amount, raw_amount, campaign_id, location, donations, url) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", $name, $photo, $position, $amount, $raw_amount, $campaign_id, $location, $donations, $url);
        }
    
        $sql_count = "select count(*) from leader_as_$num_per_page";

        //$result = db_query("select * from donations_as " . tablesort_sql($headers));
        $result = pager_query("select * from leader_as_$num_per_page " . tablesort_sql($headers), $num_per_page, 0, $sql_count);
        $rows = array();

        $convert_currency   = TRUE;
        if($type != 'walking') {
            $convert_currency   = FALSE;
        }
 
        while ($db_row = db_fetch_object($result)) {
            $our_campaign = $campaigns[$db_row->campaign_id];

            $rows[] = array(
                'data' => array(
                    array('data' => $db_row->position, 'class' => 'position'),
                    array('data' => '<img src="' . $db_row->photo . '" width="25"> <a href="' . $db_row->url . '">' . $db_row->name . '</a>', 'class' => 'name' ),
                    array('data' => $db_row->location),
                    array('data' => $db_row->donations),
                    array('data' => dw_campaigns_force_decimal($db_row->raw_amount, $our_campaign->field_dw_currency['0']['value'], $convery_currency)),
                )    
            );
        }
    
        $extra='';
        if($type == 'walking') {
            if(is_null($campaign)) {
                $extra = t('(All Locations)');
            } else {
                $extra = '(<a href="/dw/walking/location/' . $campaign->nid . '">' . $campaign->field_dw_campaign_location[0]['value'] . '</a>)';
            }
        }
    }
    //echo theme('dw_campaigns_derby_statistics', $campaign, TRUE);
    echo "<h2>" . t('Leader Board') . " $extra</h2>";
    echo theme('table', $headers, $rows);
    echo theme('pager', NULL, $num_per_page, 0);
