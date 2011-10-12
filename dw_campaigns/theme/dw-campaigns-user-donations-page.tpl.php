<?php
    $num_per_page   = 40;

    $pcp = dw_campaigns_user_get_pcp_for_campaign($user, $campaign);
    if(is_null($pcp->id)) {
        return t('you do not have any active personal campaign pages');
    }

    $supporters  = dw_campaigns_get_contributions_for_pcp($pcp, TRUE);

    $privacy        = dw_campaign_get_privacy($pcp);
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
            donations_as
            (
                name char(255),
                photo char(255),
                email char(255),
                amount float,
                location char(255),
                donationdate datetime,
                note char(255),
                is_pay_later char(255)
            )
    ";
    db_query($query);

    $rows   = array();
    foreach($supporters as $key => $supporter) {
        $image_match = '';
        

        $params                     = array();
        $params['returnFirst']      = 1;
        $params['contribution_id']  = $supporter->contribution_id;
        $contribution               = _dw_civicrm_contribution_get($params);

        $params                     = array();
        $params['contact_id']       = $supporter->contact_id;
        $params['returnFirst']      = 1;
        $contact                    = _dw_civicrm_contact_get($params);

        $row = array();

        $name       = $contact->display_name;

        
        if(!empty($supporter->pcp_roll_nickname)) {
            $name .= " (" . $supporter->pcp_roll_nickname . ")";
        }

        $photo      = _dw_campaigns_get_photo(array(), array(), 'donation-photo', $supporter->id, $image_match);
        $email      = $contact->email;
        $amount     = $supporter->amount;
        $location   = $contact->city . ', ' . $contact->state_province;
        //$date       = substr($contribution->receive_date, 0, 10);
        $date       = $contribution->receive_date;
        $note       = is_object($supporter->pcp_personal_note)?'':$supporter->pcp_personal_note;
        $pay_later  = $supporter->is_pay_later;
        
        db_query("insert into {donations_as} (name, photo, email, amount, location, donationdate, note, is_pay_later) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", $name, $photo, $email, $amount, $location, $date, $note, $pay_later);
    }

    $sql_count = "select count(*) from donations_as";

    //$result = db_query("select * from donations_as " . tablesort_sql($headers));
    $result = pager_query("select * from donations_as " . tablesort_sql($headers), $num_per_page, 0, $sql_count);

    $rows = array();
    while ($db_row = db_fetch_object($result)) {
        $tr_class = ($db_row->is_pay_later == 1) ? 'is_pay_later' : '';

        $rows[] = array(
            'data' => array(
                array('data' => '<img src="' . $db_row->photo . '" width="25"> <a href="mailto:' . $db_row->email . '">' . $db_row->name . '</a>', 'class' => 'name' ),
                array('data' => dw_campaigns_force_decimal($db_row->amount, $campaign->field_dw_currency['0']['value']), 'class' => 'amount'),
                array('data' => $db_row->location, 'class' => 'location'),
                array('data' => dw_campaigns_format_date($db_row->donationdate), 'class' => 'date'),
                array('data' => $db_row->note, 'class' => 'note'),
            ),
            'class' => $tr_class
        );
    }

?>
<div class="offline" style="float:right">
<a href="/dw/user/donations/add?ajax=1" class="fb_tall">Add Offline Donation</a>
</div>

<?php
    echo theme('table', $headers, $rows);
    echo theme('pager', NULL, $num_per_page, 0);
