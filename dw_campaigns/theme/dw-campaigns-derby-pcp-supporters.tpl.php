<?php

    if(is_null($pcp)) {
        return;    
    }

    $supporters = dw_campaigns_pcp_get_supporters($pcp);

    $headers        = array(
        array(
            'data'  => t('Name'),
            'field' => 'name',
        ),
        array(
            'data'  => t('Amount'),
            'field' => 'amount',
        ),
        array(
            'data'  => t('Location'),
            'field' => 'location',
        ),
        array(
            'data'  => t('date'),
            'field' => 'donationdate',
            'sort'  => 'desc',
        ),
        array(
            'data'  => t('Message'),
            'field' => 'note',
        )
    );


    $query = "
        CREATE TEMPORARY TABLE
            donations_as_$num_per_page
            (
                name char(255),
                photo char(255),
                email char(255),
                amount float,
                location char(255),
                donationdate date,
                note char(255),
                currency char(255)
            )
    ";
    db_query($query);

    $contributions = array();

    $rows       = array();
    foreach($supporters as $key => $supporter) {

        $params                     = array();
        $params['returnFirst']      = 1;
        $params['contribution_id']  = $supporter->contribution_id;
        $contribution               = _dw_civicrm_contribution_get($params);

        $contributions[$contribution->contribution_id] = $contribution;

        $params                 = array();
        $params['contact_id']   = $supporter->contact_id;
        $params['returnFirst']  = 1;
        $contact                = _dw_civicrm_contact_get($params);

        $image_match = '';
        if($supporter->pcp_display_in_roll == 1) {
            $name   = $supporter->pcp_roll_nickname;
            $photo  = _dw_campaigns_get_photo(array(), array(), 'donation-photo', $supporter->id, $image_match);
        } else {
            $name   = 'Anonymous';
            $photo  = _dw_campaigns_get_photo(array(), array(), 'donation-photo', 0, $image_match);
        }

        $name = "<!-- CONTRIB_ID: {$supporter->contribution_id} | CONTACT_ID: {$supporter->contact_id} -->" . $name;

        $email      = $contact->email;
        $amount     = $supporter->amount;
        $location   = $contact->city . ', ' . $contact->state_province;
        $date       = substr($contribution->receive_date, 0, 10);
        
        $note       = is_object($supporter->pcp_personal_note)?'':$supporter->pcp_personal_note;
        
        $note       .= '<!-- CONTRIBUTION_ID: ' . $supporter->contribution_id . '-->';
        
        $currency   = $contribution->currency;
     
        db_query("insert into {donations_as_$num_per_page} (name, photo, email, amount, location, donationdate, note, currency) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", $name, $photo, $email, $amount, $location, $date, $note, $currency);
    }

    $sql_count = "select count(*) from donations_as_$num_per_page";

    //$result = db_query("select * from donations_as " . tablesort_sql($headers));
    $result = pager_query("select * from donations_as_$num_per_page " . tablesort_sql($headers), $num_per_page, 0, $sql_count);

    $multi    = FALSE;
    $currency = $campaign->field_dw_currency['0']['value'];
    if($currency == 'MULTI') {
        $multi = TRUE;
    }

    $rows = array();
    while ($db_row = db_fetch_object($result)) {
        if($multi) {
            $currency = $db_row->currency;
        }
        $rows[] = array(
            'data' => array(
                array('data' => '<a><img src="' . $db_row->photo . '" width="25">' . $db_row->name . '</a>', 'class' => 'name'),
                array('data' => dw_campaigns_force_decimal($db_row->amount, $currency), 'class' => 'amount'),
                array('data' => $db_row->location, 'class' => 'location'),
                array('data' => dw_campaigns_format_date($db_row->donationdate), 'class' => 'date'),
                array('data' => $db_row->note, 'class' => 'message'),
            )    
        );
    }

    echo '<h2>' . $thisUser->displayname . ' Donors</h2>';
    echo theme('table', $headers, $rows);
    // if we are showing the big page, enable paging
    if($num_per_page!=3) {
        echo theme('pager', NULL, $num_per_page, 0);
        echo '<a href="' . dw_campaigns_get_campaign_path($campaign->nid, '/dw/users/' . $thisUser->name) . '" class="see-all">' . t('return to fundraising page') . '</a>';
    } else {    
        echo '<a href="' . dw_campaigns_get_campaign_path($campaign->nid, '/dw/users/' . $thisUser->name,  '/supporters') . '" class="see-all">' . t('see all') . '</a>';
    }
    
