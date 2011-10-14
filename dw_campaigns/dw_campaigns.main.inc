<?php

/** 
* All code is Copyright 2011 by the original authors.
* 
*  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

$dwSelectedCampaign = NULL;
$dwSelectedPcp      = NULL;
$dwSelecterUser     = NULL;
$dwSelectedCampaign = NULL;
$dwSelectedLocation = NULL;


function dw_campaigns_set_selected_location($campaign_id) {

    global $dwSelectedLocation;
    
    $dwSelectedLocation = $campaign_id;
}


function dw_campaigns_get_selected_location() {

    global $dwSelectedLocation;
    
    // we probably want to cascade down and see if they are logged in, and if so where their pcp location is
    if(is_null($dwSelectedLocation)) {
        $dwSelectedLocation = isset($_SESSION['last_location'])?$_SESSION['last_location']:NULL;
    }
    
    return $dwSelectedLocation;
}

/**
 *  Campaigns are drupal content nodes of type 'Derby' or (in expansion) Walking - we store the actual full node
 */
function dw_campaigns_set_selected_campaign($campaign) {

    global  $dwSelectedCampaign;
    if($campaign) {
        $dwSelectedCampaign = clone $campaign;
    }
}

/**
 *  Returns the Derby/Walking node object
 */
function dw_campaigns_get_selected_campaign() {

    global  $dwSelectedCampaign;
    
    return $dwSelectedCampaign;
}

function dw_campaigns_set_selected_type($type) {

    global  $dwSelectedType;
    
    $dwSelectedType = $type;
}

function dw_campaigns_get_selected_type() {

    global  $dwSelectedType;
    
    return $dwSelectedType;
}


/**
 *  PCPs are CiviCRM Personal Campaign Pages - we store a copy of the object
 */
function dw_campaigns_get_selected_pcp() {
    
    global  $dwSelectedPcp;
    
    return $dwSelectedPcp;
}

/**
 *  Returns the PCP Object
 */
function dw_campaigns_set_selected_pcp($pcp) {
    
    global  $dwSelectedPcp;
    
    $dwSelectedPcp = $pcp;
}

/**
 *  Users are drupal user objects (this is so we can get/set the 'owner' of a page)
 */
function dw_campaigns_get_selected_user() {
    
    global  $dwSelectedUser;
    
    return $dwSelectedUser;
}

/**
 *  Returns the Drupal user objects (this is so we can get/set the 'owner' of a page)
 */
function dw_campaigns_set_selected_user($user) {
    
    global  $dwSelectedUser;
    
    $dwSelectedUser = $user;
}


function dw_campaigns_user_is_email_unique($email) {
    //check email
    $result 	= db_query("SELECT * FROM {users} u WHERE mail = '%s'", $email);
    $user       = db_fetch_object($result);
    return ($user->uid == 0);
}

function dw_campaigns_user_is_email_unique_civi($email) {
    
    return true; // they didn't want this check
    
    $params = array(
        'email'    => $email,
        'returnFirst'   => 1
    );
    $res    = _dw_civicrm_contact_get($params);
    if(is_null($res->contact_id)) {
        return true;
    }
    return false;
}



/**
 * This function acts as glue between a drupal node and the civicrm backend.  When a new dw_campaign content is created in drupal
 * this function creates a contribution page in civi that corresponds with it
 */
function dw_campaigns_create_contribution_page_for_campaign($node) {
    $hasPage = false;
    
    $node = (is_object($node)) ? $node : node_load($node);

    //if we are creating a page, we have to create a contribution type first
    $contributionType   = dw_campaigns_create_contribution_type_for_campaign($node);
  
    
    //check if we are updating, id will come back blank if nothing was found
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($node);
    if(isset($contributionPage->id) && !is_null($contributionPage->id) && !empty($contributionPage->id)) {
        $params['id']       = $contributionPage->id;
        $hasPage            = true;
    }
   
   
    
    $params['title']                    = $node->title;
    $params['contribution_type_id']     = $contributionType->id;
    $params['intro_text']               = $node->body;
    $params['footer_text']              = '';
    $params['is_for_organization']      = '0';
    $params['for_organization']         = 'I am contributing on behalf of an organization.';
    $params['goal_amount']              = $node->field_dw_fundraising_goal[0]['value'];
    $params['is_active']                = ($node->field_dw_campaign_status[0]['value'] == 'active') ? '1' : '0';
    $params['honor_block_title']        = '';
    $params['honor_block_text']         = '';
    $params['start_date']               = '';
    $params['start_date_time']          = '';
    $params['end_date']                 = '';
    $params['end_date_time']            = '';
    
    
    // LOOK - this is being sent to civi, why would it want our drupal user id?
    global $user;

    $params['created_id']               = $user->uid;
    $params['created_date']             = date('YmdHis');
    
    
    $params['is_credit_card_only']      = '';
    $params['honor_block_is_active']    = '';
    $params['is_allow_other_amount']    = '1';
    $params['min_amount']               = '0';
    $params['max_amount']               = '99999';
    
    
    $params['amount_block_is_active']   = 1;
    $params['is_monetary']              = 1;
    $params['payment_processor_id']     = 1;
    

    $params['is_email_receipt']         = '1';    
    $params['thankyou_title']           = $node->field_dw_thankyou_title[0]['value'];
    $params['thankyou_text']            = $node->field_dw_thankyou_text[0]['value'];
    $params['thankyou_footer']          = $node->field_dw_thankyou_footer[0]['value'];
    $params['receipt_from_name']        = $node->field_dw_receipt_from_name[0]['value'];
    $params['receipt_from_email']       = $node->field_dw_receipt_from_email[0]['value'];
    $params['receipt_text']             = $node->field_dw_receipt_text[0]['value'];
    $params['cc_receipt']               = $node->field_dw_cc_receipt[0]['value'];
    $params['bcc_receipt']              = $node->field_dw_bcc_receipt[0]['value'];

    $params['currency']                 = $node->field_dw_currency[0]['value'];

    $page = _dw_civicrm_contributionpage_create($params);

    // this saves a record so we can relate node ids to civi ids (from both sides)    
    _dw_save_node_to_civi_campaign_glue($node->nid, $page->id);

    
    // if this is updating an existing contribution page we need to update the titles in all the pcps (not the pcpblock) that are tied to that contribution page
    if($hasPage) {
        $params = array();
        $params['title']                = $node->title;
        $params['contribution_page_id'] = $page->id;
    
//        _dw_civicrm_pcp_sync_titles($params);
    }

    //enable pcp's on this contribution page by inserting a new record into pcpblock
    //see if a pcpblock already exists so we can update it
    $params = array();
    $params['entity_id']    = $page->id;
    $params['pcpBlock'] = true;
    $params['returnFirst']  = 1;

    $pcpBlock = _dw_civicrm_pcp_get($params);

    // create or update the pcpBlock
    $params                          = array();
    if(isset($pcpBlock->id) && !is_null($pcpBlock->id) && !empty($pcpBlock->id)) {
        $params['id']                    = $pcpBlock->id;
    }
    $params['is_active']             = 1;
    $params['is_approval_needed']    = 1;
    $params['supporter_profile_id']  = 2;
    $params['is_tellfriend_enabled'] = 1;
    $params['tellfriend_limit']      = 9;
    $params['link_text']             = 'Donate Now';
    $params['notify_email']          = '';
    $params['entity_id']             = $page->id;
    $params['entity_table']          = 'civicrm_contribution_page';
    
    $params['pcpBlock']              = true;
    
    $pcpBlock = _dw_civicrm_pcp_create($params);
    // we don't return anything
    
    if($node->field_dw_create_event[0]['value'] != 0 ) {
        // create the event also
        $event = dw_campaigns_create_event_registration_for_campaign($node, $contributionType->id);
    
        _dw_save_node_to_civi_event_glue($node->nid, $event->event_id);
    }

}

/**
 * Creates a contribution type from a campaign node
 * 
 * @param stdClass $node
 * @return CRM_Contribute_DAO_ContributionType the contribution type id
 */
function dw_campaigns_create_contribution_type_for_campaign($node) {
    $node = (is_object($node)) ? $node : node_load($node);

    //determine if we are updating by checking if a page already exists for the node
    //if one does, set the id of the type_id so it's saved
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($node);

    $params = array();
    //$params['contributionType'] = $contributionPage->contribution_type_id;
    if(isset($contributionPage->contribution_type_id) && !is_null($contributionPage->contribution_type_id) && !empty($contributionPage->contribution_type_id)) {
        
        $params['id']               = $contributionPage->contribution_type_id; 
    }
    $params['name']             = "Campaign " . $node->nid . ": " . $node->title;
    $params['description']      = $node->body;
    $params['is_deductible'] 	= 1;
    $params['is_active'] 	= 1;


    $type = _dw_civicrm_contributiontype_create($params);
    return $type;
}



/**
 * Passing this function a node or node id will cause it to
 * search for any associated contribution page, if one is found, it is returned
 * 
 * @param stdObj $node
 * @return mixed either the page it found or null
 */
function dw_campaigns_get_contribution_page_for_campaign($node) {
    static $pages = array();
    
    $node = (is_object($node)) ? $node : node_load($node);
    
    $res = new stdClass;
    
    if(!isset($pages[$node->nid])) {
    
        $query  = sprintf("SELECT contribution_page_id FROM dw_campaigns_campaigns_to_contribution_pages WHERE nid = '%s'", $node->nid);
        $result = db_query($query);

        if(count($result) == 0 ) {
            return $res;
        }
      
        $row        = db_fetch_object($result);
        
        // this isn't necessary most of the time, since we don't care about anything BUT the page id :)
        $page       = dw_campaigns_get_contribution_page_from_id($row->contribution_page_id);
        
        $pages[$node->nid] = $page;
  
    }
    
    return $pages[$node->nid];
}

// TODO
function dw_campaigns_get_event_registration_for_campaign($node) {
    static $pages = array();

    $res = new stdClass;

    $node = (is_object($node)) ? $node : node_load($node);
    
    if(!isset($pages[$node->nid])) {
    
        $query  = sprintf("SELECT event_page_id FROM dw_campaigns_campaigns_to_contribution_pages WHERE nid = '%s'", $node->nid);
        $result = db_query($query);
        
        if(count($result) == 0 ) {
            return $res;
        }

        $row        = db_fetch_object($result);

        // get event by event id
        $event      = _dw_civicrm_event_get(array('id' => $row->event_page_id));

        $pages[$node->nid] = $event;
    }
    
    return $pages[$node->nid];
}

function dw_campaigns_get_event_page_id_from_campaign($campaign) {
    $results    = db_query("SELECT event_page_id FROM dw_campaigns_campaigns_to_contribution_pages WHERE nid = '%s'", $campaign->nid);
    $row        = db_fetch_object($results);
    
    return $row->event_page_id;    
}

function dw_campaigns_get_event_participants_count($campaignId, $pcpId, $contactId = 0) {
    if($pcpId == 0 && $contactId == 0) {
        return NULL;
    }
    
    if($pcpId > 0) {
        $results    = db_query("SELECT participants FROM dw_campaigns_event_participants WHERE campaign_id = '%s' and pcp_id = '%s'", $campaignId, $pcpId);
    } else {
        $results    = db_query("SELECT participants FROM dw_campaigns_event_participants WHERE campaign_id = '%s' and contact_id = '%s'", $campaignId, $contactId);
    }

    $row        = db_fetch_object($results);

    return $row->participants;
}

function dw_campaigns_set_event_participants_count($campaignId, $pcpId, $num_attending = 1, $participation_type = 'adult_pledge', $contactId = 0) {
    $campaignId  = intval($campaignId);
    $pcpId       = intval($pcpId);
    $contactId   = intval($contactId);

    if($pcpId == 0 && $contactId == 0) {
        drupal_set_message(t('To add participants you must provide a personal campaign page id or a contact id'), 'error');
        return;
    }
   
    if($pcpId > 0) {
        $results    = db_query("DELETE from dw_campaigns_event_participants WHERE campaign_id = '%s' and pcp_id = '%s'", $campaignId, $pcpId);
    } else {
        $results    = db_query("DELETE from dw_campaigns_event_participants WHERE campaign_id = '%s' and contact_id = '%s'", $campaignId, $contactId);
    }
 
    db_query("insert into dw_campaigns_event_participants (campaign_id, pcp_id, participants, participant_type, contact_id) VALUES('%s', '%s', '%s', '%s', '%s')", $campaignId, $pcpId, $num_attending, $participation_type, $contactId);
    $a = sprintf("insert into dw_campaigns_event_participants (campaign_id, pcp_id, participants, participant_type, contact_id) VALUES('%s', '%s', '%s', '%s', '%s')", $campaignId, $pcpId, $num_attending, $participation_type, $contactId);
    file_put_contents(drupal_get_path('module','dw_campaigns') . "/event_participants.inc", $a, FILE_APPEND);
}

function dw_campaigns_get_contribution_page_from_id($id) {
    
    static $pages = array();
    
    if(!isset($pages[$id])) {
        $params['id'] = $id;
        
        $page = _dw_civicrm_contributionpage_get($params);
        
        $pages[$id] = $page;
    }
    
    return $pages[$id];
}


/**
 * Loads the personal campaign page for a user for a specific campaign
 * @param stdClass $user the user we are checking for
 * @param stdClass $campaign the campaign node or node id
 * @return stdClass
 */
function dw_campaigns_user_get_pcp_for_campaign($user, $campaign) {
    $campaign = (is_object($campaign)) ? $campaign : node_load($campaign);

    $params = array();
    $params['pcpBlock'] = 0;
    
    $page = dw_campaigns_get_contribution_page_for_campaign($campaign);

    if(is_null($page->id)) {
        return $page; // this is a failure to lookup!
    }
    
    $params['contribution_page_id'] = $page->id;
    $contactId                      = _dw_campaigns_user_get_contact_id($user);
    $params['contact_id']           = $contactId;
    $params['returnFirst']          = 1;
    /*
    // test set - returns 1 record //
    $params['contribution_page_id'] = 1;
    $params['contact_id'] = 1737;
    */
    $pcp = _dw_civicrm_pcp_get($params);    
    return $pcp;  
}

function dw_campaigns_user_get_pcp_for_campaign_by_contactid($contactId, $campaign) {
    $campaign = (is_object($campaign)) ? $campaign : node_load($campaign);

    $params = array();
    $params['pcpBlock'] = 0;
    
    $page = dw_campaigns_get_contribution_page_for_campaign($campaign);

    if(is_null($page->id)) {
        return $page; // this is a failure to lookup!
    }
    
    $params['contribution_page_id'] = $page->id;
    $params['contact_id']           = $contactId;
    $params['returnFirst']          = 1;

    $pcp = _dw_civicrm_pcp_get($params);    
    return $pcp;  
}



/**
 * Finds the first PCP that is related to a user (for walking)
 * @param stdClass $user the user we are checking for
 * @return stdClass
 */
function dw_campaigns_user_get_pcp_for_walking($user) {


    $campaigns  = dw_campaigns_get_campaigns(TRUE);
    $campaign_values = array_keys($campaigns);

    $params = array();
    $params['pcpBlock'] = 0;
    
    $contactId                      = _dw_campaigns_user_get_contact_id($user);
    $params['contact_id']           = $contactId;
    //$params['returnFirst']          = 1;

    $pcps   = _dw_civicrm_pcp_get($params);    

    foreach($pcps as $pcp) {
        if(in_array($pcp->contribution_page_id, $campaign_values)) {
            return $pcp;  
        }
    }

    return NULL;
}

// takes pcp id, returns pcp (not PCPblock)
function dw_campaigns_user_get_pcp_by_id($pcpId)
{
    $params = array();
    $params['pcpBlock']     = 0;
    $params['id']           = $pcpId;
    $params['returnFirst']  = 1;
    $pcp = _dw_civicrm_pcp_get($params);
    
    return $pcp;
}

// takes pcpBlock id, returns pcpBlock
function dw_campaigns_user_get_pcpblock_by_id($pcpBlockId)
{
    $params = array();
    $params['pcpBlock']     = 1;
    $params['id']           = $pcpBlockId;
    $params['returnFirst']  = 1;

    $pcp = _dw_civicrm_pcp_get($params);
    
    return $pcp;
}

function dw_campaigns_get_campaign_from_contribution_page($contributionPage) {
    $results    = db_query("SELECT nid FROM dw_campaigns_campaigns_to_contribution_pages WHERE contribution_page_id = '%s'", $contributionPage->id);
    $row        = db_fetch_object($results);
    
    return node_load($row->nid);    
}

/**
 *   NOTE - this is where we should add the check for "is disabled" so next year goes smooth
 */
function dw_campaigns_get_campaigns($keyedByContributionPage = FALSE) {
    static $campaigns = NULL;
    static $campaigns_cp = NULL;
    if(is_null($campaigns)) {
        $results    = db_query("SELECT nid, contribution_page_id FROM dw_campaigns_campaigns_to_contribution_pages where active='Y'");
    
        while($row = db_fetch_object($results)) {
            $node = node_load($row->nid);
            if(is_object($node)) {
                $campaigns[] = $node;
                $campaigns_cp[$row->contribution_page_id] = $node;
            }
        }
    }
    if($keyedByContributionPage) {
        return $campaigns_cp;
    } else {
        return $campaigns;
    }
}

function dw_campaigns_get_campaign_ids() {
    static $campaigns = NULL;
    if(is_null($campaigns)) {
        $results    = db_query("SELECT nid FROM dw_campaigns_campaigns_to_contribution_pages where active='Y'");
    
        while($row = db_fetch_object($results)) {
            $campaigns[] = $row->nid;    
        }
    }

    return $campaigns;
}

function dw_campaigns_get_contribution_total_for_campaign($node) {

    $node = (is_object($node)) ? $node : node_load($node);
  
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($node);
    
    $params                         = array();
    $params['contribution_page_id'] = $contributionPage->id;
    $params['rowCount']             = 999999;

    $contributions                  = _dw_civicrm_contribution_get($params);
    $total=0;
    foreach($contributions as $contribution)
    {
            $total += $contribution->total_amount;
    }


    return $total;

}

/**
 * Get the "soft" contributions to PCP pages
 * @param PCP_Object the pcp object
 * @return array the contributions if any were found
 */
function dw_campaigns_get_contributions_for_pcp($pcp, $extended = FALSE) {
    
    static $contributions = array();
    $pcp_id = $pcp->id;

    if(!isset($contributions[$pcp_id])) {
        $contributions[$pcp_id] = array();
        
        $params = array();
        $params['pcp_id']   = $pcp_id;
        $params['rowCount'] = 999999;
        
        $softcontributions  = _dw_civicrm_contributionsoft_get($params);
        
        if($extended) { 
            $params                          = array();
            $params['contribution_page_id']  = $pcp->contribution_page_id;
            $params['rowCount']              = 999999;
            $full_contributions              = _dw_civicrm_contribution_get($params);
        }

        foreach($softcontributions as $softcontribution) {

            if($extended) {
                $key                     = '_' . $softcontribution->contribution_id;
                $contribution            = $full_contributions->$key;
                if(!is_null($contribution)) {
                    $softcontribution    = (object) array_merge((array) $contribution, (array) $softcontribution);
                } else {
                    file_put_contents("/tmp/contribution_merge_misses.txt", $softcontribution->contribution_id . "\n", FILE_APPEND);
                }
            }

            $contributions[$pcp_id][] = $softcontribution;
        }
    }
    
    return $contributions[$pcp_id];
    
}

/**
 * Tells you how much has been dontated for this pcp
 */
function dw_campaigns_get_contribution_total_for_pcp($pcp, $convertNONUSD = FALSE) {
   // $contributions  = dw_campaigns_get_contributions_for_pcp($pcp, TRUE);
    $softcontributions  = dw_campaigns_get_contributions_for_pcp($pcp);
    
    $total          = 0;
    $count          = 0;
    $offline        = 0;

    // lookup regular contributions
    $params                             = array();
    $params['contribution_page_id']     = $pcp->contribution_page_id;
    $params['rowCount']                 = 999999;
    $params['contribution_status_id']   = 2; // pending only
    $contributions                      = _dw_civicrm_contribution_get($params);


    foreach($softcontributions as $softcontribution) {
        $key = '_' . $softcontribution->contribution_id;
        $contribution = $contributions->$key;

        if(!is_null($contribution)) {
            $offline += $softcontribution->amount;
        }

        $amount =  $softcontribution->amount;
        if($convertNONUSD) {
            $amount = dw_campaigns_convert_to_usd($softcontribution->currency, $amount);
        }
        $total += $amount;
        $count++;
    }
    // save the total number of donations
    dw_campaigns_set_contribution_count_for_pcp($pcp, $count);

    return array('total' => $total, 'count' => $count, 'offline' => $offline);

}
/**
 * This function is named poorly, we can both get and set with it
 */
function dw_campaigns_set_contribution_count_for_pcp($pcp, $count = 0) {
    return dw_campaigns_get_contribution_count_for_pcp($pcp, $count);
}

function dw_campaigns_get_contribution_count_for_pcp($pcp, $count = NULL) {
    static $counts = array();
    if(!is_null($count)) {
        $counts[$pcp->id] = $count;
    }
    
    if(!isset($counts[$pcp->id])) {
        $res   = dw_campaigns_get_contribution_total_for_pcp($pcp);
        $total = $res['total'];
        $count = $res['count'];
        $counts[$pcp->id] = $count;
    }
    
    return $counts[$pcp->id];
}

function dw_campaigns_valid_type($campaign) {
    if($campaign->type == 'dw_campaigns_derby' || $campaign->type == 'dw_campaigns_walking') {
        return TRUE;
    }
    
    return FALSE;
}

/**
 * Looks up a users CiviCRM contact id
 */
function _dw_campaigns_user_get_contact_id($user) {
    // TODO - lookup contact id from user id
    $result     = db_query("SELECT civi_contact_id from dw_campaigns_drupal_civi_contact_mapping where drupal_id = '%s'", $user->uid);
    $thisUser   = db_fetch_object($result);
    
    if(!$thisUser) {
        return NULL;
    } else {
        return $thisUser->civi_contact_id;
    }
}

/**
 * Looks up a CiviCRM contact's drupal id
 */
function _dw_campaigns_contact_id_get_user($contact_id) {
    static $contacts = array();
    
    if(!isset($contacts[$contact_id])) {
        // TODO - lookup contact id from user id
        $result     = db_query("SELECT drupal_id from dw_campaigns_drupal_civi_contact_mapping where civi_contact_id = '%s'", $contact_id);
        $thisUser   = db_fetch_object($result);
        
        if(!$thisUser) {
            $contacts[$contact_id] = NULL;
        } else {
            $contacts[$contact_id] = $thisUser->drupal_id;
        }
    }
    
    return $contacts[$contact_id];
}

/**
 * Used for verifying a username is taken or not
 * 
 * @param boolean $echoResults
 * @return string 'found' | 'not-found'
 */
function dw_campaigns_username_search_ajax($username = NULL, $echoResults = TRUE) {
    
    global $user;

    $username   = !is_null($username) ? $username : $_POST['username'];
    
    //they are logged in and they entered their own username which we want to allow
    if($user->uid > 0 && $user->name == $username) {
        echo 'not-found';exit;
    }
        
    $result 	= db_query("SELECT * FROM {users} u WHERE name = '%s'", $username);
    $thisUser   = db_fetch_object($result);

    // $thisUser will be false if no lines return;
    $results = ($thisUser) ? 'found' : 'not-found';

    if($echoResults) {
        
        echo $results;
        echo '|';
        echo dw_campaigns_make_url($username);

        exit;
    } 
    
    return $results;
}

function dw_campaigns_campaign_member_search_autocomplete($campaign_id, $username) {

	$data = dw_campaigns_campaign_search_pcp_search($campaign_id, $username);
	
	$return = array();

	foreach($data as $key => $value) {
		$return[$value->sort_name] = $value->display_name;
	}

	return drupal_json($return);
}

function dw_campaigns_ajax_position($pcp_id, $amount) {
    $pcp    = dw_campaigns_user_get_pcp_by_id($pcp_id);
    
    echo theme('dw_campaigns_ajax_position', $pcp, $amount);
    die;
}

function dw_campaigns_ajax_currencyformat($amount = '0.00', $currency = 'USD') {
    echo dw_campaigns_force_decimal($amount, $currency);
    die;
}


function dw_campaigns_create_event_registration_for_campaign($node, $contribution_type_id = 4) {

    //check if we are updating, id will come back blank if nothing was found
    $event   = dw_campaigns_get_event_registration_for_campaign($node);
    if(isset($event->id) && !is_null($event->id) && !empty($event->id)) {
        $params['id']       = $event->id;        
    }
    
    $params['title']                            = $node->title;
    $params['event_type_id']                    = '3'; // this is hardcoded for FPWR 'Derby' style
    $params['description']                      = $node->body;
    $params['summary']                          = $node->body;
// TODO - evaluate which of these should be configurable, some of these settings are from 2010 FPWR annual conference
    $params['participant_listing_id']           = NULL; // ???
    $params['is_public']                        = 1;
    $params['start_date']                       = '';
    $params['end_date']                         = '';   
    $params['is_online_registration']           = 1;
    $params['registration_link_text']           = 'Register Now';
    $params['registration_start_date']          = '';
    $params['registration_end_date']            = '';
    $params['event_full_text']                  = 'This event is currently full.';
    // taken from FPWR 'running for FPWR and Ayden Jane'
    $params['is_monetary']                      = 1;
    $params['contribution_type_id']             = $contribution_type_id;
    $params['payment_processor_id']             = 1;
    $params['is_map']                           = 1;
    $params['is_active']                        = 1;
    $params['fee_label']                        = 'Event Fee(s)';
    
    $params['is_show_location']                 = 0; // this requires the creation of a 'location' which adds complexity here, for now if someone needs it they can just edit the event we insert
    $params['loc_block_id']                     = '';
    
    $params['default_role_id']                  = 1;
    $params['intro_text']                       = '';
    $params['footer_text']                      = '';
    $params['confirm_title']                    = 'Confirm Your Registration Information';
    $params['confirm_text']                     = 'Please confirm your registration information.';
    $params['confirm_footer_text']              = '';
    $params['is_email_confirm']                 = 1;
    $params['confirm_email_text']               = 'Thank you for your registration for the ' . $params['title'];
    $params['confirm_from_name']                = '';
    $params['confirm_from_email']               = '';
    $params['bcc_confirm']                      = '';
    $params['thankyou_title']                   = 'Thank you for registering';
    $params['thankyou_text']                    = 'Thank you for your registration for the ' . $params['title'];
    $params['thankyou_footer_text']             = '';
    $params['is_pay_later']                     = 0;
    $params['pay_later_text']                   = 'I will send payment by check';
    $params['pay_later_receipt']                = '';
    $params['is_multiple_registrations']        = 0; // TODO - maybe let this be set
    $params['allow_same_participant_emails']    = 0; // TODO maybe let this be set
    $params['has_waitlist']                     = 0;
    $params['requires_approval']                = NULL;
    $params['expiration_time']                  = NULL;
    $params['waitlist_text']                    = '';
    $params['approval_req_text']                = '';
    $params['is_template']                      = 0;
    $params['currency']                         = $node->field_dw_currency[0]['value'];
    
    // address information
    $params['location_type_id']          = 1;
    $params['is_primary']                = 1;
    $params['street_address']            = $node->field_dw_address[0]['street'];
    $params['city']                      = $node->field_dw_address[0]['city'];
    $params['postal_code']               = $node->field_dw_address[0]['postal_code'];
    $params['supplemental_address_2']    = '';
    $params['postal_code_suffix']        = '';
    $params['geo_code_1']                = ''; 
    $params['geo_code_2']                = '';
    
    $event = _dw_civicrm_event_create($params);

    return $event;
}

function dw_campaigns_create_event_participant($user, $event = NULL, $otherParams) {

    if(is_null($event)) {
        if(!isset($otherParams['id'])) {
            drupal_set_message(t('you _MUST_ provide event_id to add a participant'), 'error');
            return;
        } else {
            $event_id   = $otherParams['id'];
            $event      = _dw_civicrm_event_get(array('id' => $event_id));
        }
    } else {
        $event_id = $event->id;
    }
    
    if(is_null($event_id)) {
        echo "failed to locate event - contact tech support";
        die;
    }
    //print_r($event);die;
    // I have no idea what I'm doing in here..
    /*
    [contact_id] => 1308
    [contact_type] => Individual
    [sort_name] => (removed)
    [display_name] => (removed)
    [event_id] => 8
    [event_title] => Go the Extra Mile for PWS
    [event_start_date] => 2010-05-15 08:30:00
    [event_end_date] => 2010-05-15 14:00:00
    [participant_id] => 6
    [participant_fee_level] => ,Race Registration,
    [participant_fee_amount] => 18.00
    [participant_fee_currency] => USD
    [event_type] => Fundraiser
    [participant_status_id] => 1
    [participant_status] => Registered
    [participant_role_id] => 1
    [participant_register_date] => 2010-03-30 11:04:42
    [participant_source] => Online Event Registration: Go the Extra Mile for PWS
    [participant_is_pay_later] => 0
    [participant_is_test] => 0
    */
    $contactId  = NULL;
    
    // this will add the current logged in user to the event specified
    if(isset($otherParams['addLoggedIn']) && $otherParams['addLoggedIn']) {
        $contactId      = _dw_campaigns_user_get_contact_id($user);
    } else {
        // I think we just add them as contacts and hope civi sorts it out?
        $params     = array();
        /*
        $params['first_name']       = _temp_rand_string(5); //$values['first_name'];
        $params['last_name']        = _temp_rand_string(8); //$values['last_name'];
        $params['email']            = _temp_rand_string(20); //$values['email'];
        */
        $params['first_name']       = 'liz_first'; //$values['first_name'];
        $params['last_name']        = 'liz_last'; //$values['last_name'];
        $params['email']            = 'liz_email'; //$values['email'];
        $params['contact_type']     = 'Individual';
        $params['returnFirst']      = 1;

        $contact_temp   = _dw_civicrm_contact_get($params);

        if(!is_null($contact_temp->contact_id)) {
            echo "did not insert";
            $contact        = $contact_temp;
        } else {
            unset($params['returnFirst']);
            echo "did insert";
            $contact        = _dw_civicrm_contact_create($params);

        }
        $contactId      = $contact->contact_id;
    }

    if(is_null($contactId)) {
        drupal_set_message(t('We were unable to create or find a contact record - so adding to an event is impossible'), 'error');
        return;
    }

    echo "Searching contact $contactId<br>";
    $searchParams               = array();
    $searchParams['contact_id'] = $contactId;
    $searchParams['event_id']   = $event_id;  
    $participant                = _dw_civicrm_participant_get($searchParams);
    
    if(is_null($participant->participant_id)) {
        drupal_set_message("user does not exist, adding", 'error');
        // this person does not exist yet
        if(!is_null($contactId)) {
            drupal_set_message("we have no contact id", 'error');
            $participant->contact_id                    = $contactId;
        }
        $participant->event_id                      = $event_id;
        $participant->participant_fee_level         = '';
        $participant->participant_fee_amount        = '';
        $participant->participant_fee_currency      = '';
        $participant->participant_status_id         = 1;
        $participant->participant_status            = 'Registered';
        $participant->participant_role_id           = 1;
        $participant->participant_register_date     = date('Y-m-d H:i:s');
        $participant->participant_source            = 'Online Event Registration: ' . $event->title;
        $participant->participant_is_pay_later      = 0;
        $participant->participant_is_test           = 0;
    } else {
        // I don't know how we're handling updating..
        drupal_set_message("This person is already a participant in this event", 'notice');
    }
    
    $params = get_object_vars($participant);
    
    $res = _dw_civicrm_participant_create($params);
    print_r($res);
    drupal_set_message("participant added", 'status');
    return $res->participant_id;
}

function _temp_rand_string($strlen = 6) {
    $accum = '';
    for ($i=0; $i<$strlen; $i++) { 
        $d=rand(1,30)%2; 
        $accum .= $d ? chr(rand(65,90)) : chr(rand(48,57)); 
    }
    return $accum;
}

function dw_campaigns_display_event_details($node) {
    $node = (is_object($node)) ? $node : node_load($node);
    
    
}

function _dw_campaigns_get_pcp_by_drupal_id($user) {
    static $pcp_list = array();

    // in some situations we don't have the actual campaign object, just its id
    $contact = _dw_civicrm_contact_get_from_user($user);
    if(!isset($contact->contact_id)) {
        return array();
    }
    
    if(!isset($pcp_list[$contact->contact_id])) {    
        
        $params = array();
        $params['pcpBlock']             = 0;
        $params['contact_id']           = $contact->contact_id;
        $pcps                           = _dw_civicrm_pcp_get($params);

        $pcp_list[$contact->contact_id] = $pcps;
    }
    
    return $pcp_list[$contact->contact_id];
}


function _dw_campaigns_get_pcps_for_campaign($campaign) {
    static $pcp_list = array();

    // in some situations we don't have the actual campaign object, just its id
    $nid = (is_object($campaign)) ? $campaign->nid : $campaign;

    if(!isset($pcp_list[$nid])) {    
        // dw_campaigns_get_contribution_page_for_campaign understands both campaigns and campaid_ids
        $contribution_page              = dw_campaigns_get_contribution_page_for_campaign($campaign);
        if(empty($contribution_page->id))  {
            return array();
        }
        $params = array();
        $params['rowCount']             = 999999;
        $params['pcpBlock']             = 0;
        $params['contribution_page_id'] = $contribution_page->id;
        $pcps                           = _dw_civicrm_pcp_get($params);

        $pcp_list[$nid]       = $pcps;
    }
    
    return $pcp_list[$nid];
}

function _dw_campaigns_campaign_total($campaign) {
    
    $contribution_page              = dw_campaigns_get_contribution_page_for_campaign($campaign);

    $goal                           = $contribution_page->goal_amount;
    $total_raised                   = 0;
    $pcp_count                      = 0;
    $sponsor                        = 0;
  
    $pcps                           = _dw_campaigns_get_pcps_for_campaign($campaign);

    $currency                       = $campaign->field_dw_currency['0']['value'] ;
    $convert                        = ($currency == 'MULTI') ? TRUE : FALSE;
    
    foreach($pcps as $pcp) {
        $res                        = dw_campaigns_get_contribution_total_for_pcp($pcp, $convert);
        extract($res);
        $total_raised               += $total;
if(strstr($pcp->title, "General Donation")) {

} else {
        $pcp_count                  += 1;
}
    }

    if(isset($campaign->field_dw_other_donations['0']['value'])) {
        $sponsor       = $campaign->field_dw_other_donations['0']['value'];
        $total_raised += $sponsor;
    }

    return array('goal' => $goal, 'raised' => $total_raised, 'pcp_count' => $pcp_count, 'sponsor' => $sponsor);
}

function  _dw_campaigns_campaigns_total() {
    $goal = 0;
    $total_raised = 0;
    
    $campaigns  = dw_campaigns_get_campaigns();
    if(count($campaigns) > 0 ) {
        foreach($campaigns as $campaign) {
            $res            = _dw_campaigns_campaign_total($campaign);
            $amount         = $res['raised'];
            $goal           += $res['goal'];
            $total_raised   += $amount;
            $total_usd      += dw_campaigns_convert_to_usd($campaign->field_dw_currency['0']['value'], $amount);
        }
    }
    
    return array('goal' => $goal, 'raised' => $total_raised, 'raised_usd' => $total_usd);
}


function  _dw_campaigns_campaigns_campaign_total() {
    $totals         = array();
    $usd_totals     = array();
    $pcp_counts     = array();

    $campaigns      = dw_campaigns_get_campaigns();
    $campaigns_l    = array(); 
   
    if(count($campaigns) > 0) {
        foreach($campaigns as $campaign) {
            $nid                = $campaign->nid;
    
            $res                = _dw_campaigns_campaign_total($campaign);
    
            $total              = $res['raised'];
            $totals[$nid]       = $total;

            $usd_total          = dw_campaigns_convert_to_usd($campaign->field_dw_currency['0']['value'], $res['raised']);
            $usd_totals[$nid]   = $usd_total;
    
            $pcp_count          = $res['pcp_count'];
            $pcp_counts[$nid]   = $pcp_count;
    
            $campaigns_l[$nid]  = $campaign;
        }
    }
    arsort($totals, SORT_NUMERIC);
    arsort($usd_totals, SORT_NUMERIC);
    
    return array('totals' => $totals, 'usd_totals' => $usd_totals, 'campaigns' => $campaigns_l, 'pcp_counts' => $pcp_counts);
}


/**
 * Returns a list of all pcps for a specific campaign ordered by total
 */
function _dw_campaigns_campaign_leaders_list($campaign) {
    static $leaders = array();

    if(!isset($leaders[$campaign->nid])) {
        
        $pcps                           = _dw_campaigns_get_pcps_for_campaign($campaign);
        $totals                         = array();
        $counts                         = array();

        $currency                       = $campaign->field_dw_currency['0']['value'] ;
        $convert                        = ($currency == 'MULTI') ? TRUE : FALSE;
        
        foreach($pcps as $pcp) {
            $res                        = dw_campaigns_get_contribution_total_for_pcp($pcp, $convert);
            $totals[$pcp->contact_id]   = $res['total'];
            $counts[$pcp->contact_id]   = $res['count'];
        }
        
        arsort($totals, SORT_NUMERIC);
        arsort($counts, SORT_NUMERIC);
        
        $leaders[$campaign->nid]    = array('totals' => $totals, 'counts' => $counts);
    }
    return $leaders[$campaign->nid];
}


function _dw_campaigns_campaign_leaders_list_fake($campaign, $pcp_id, $amount) {

    $currency                       = $campaign->field_dw_currency['0']['value'] ;
    $convert                        = ($currency == 'MULTI') ? TRUE : FALSE;

    $pcps                               = _dw_campaigns_get_pcps_for_campaign($campaign);
    
    foreach($pcps as $pcp) {
        $res                            = dw_campaigns_get_contribution_total_for_pcp($pcp, $convert);

        if($pcp_id == $pcp->id) {
            $totals[$pcp->contact_id]   = $amount;
            $counts[$pcp->contact_id]   = $res['count']+1;
        } else {
            $totals[$pcp->contact_id]   = $res['total'];
            $counts[$pcp->contact_id]   = $res['count'];
        }
    }
    
    arsort($totals, SORT_NUMERIC);
    arsort($counts, SORT_NUMERIC);
    
    return array('totals' => $totals, 'counts' => $counts);
}


/**
 * returns 'top X' leaders for a given campaign
 * returns name (displayname), drupal id, address to users page, total amount of money raised, and total number of donations
 */
function _dw_campaigns_campaign_leaders($campaign = NULL, $count = 3) {

    $res    = _dw_campaigns_campaign_leaders_list($campaign);
    extract($res);

        
    $campaign_location  = $campaign->field_dw_campaign_location[0]['value'];

    $image_match    = '';
    $image_params   = array(
        'w'                 => 45,
        'contribution'      => true,
    );

    foreach($totals as $contact_id => $total)
    {
        if($count==0)
                break;

        if(dw_campaigns_hide_contact($contact_id)) {
            continue; 
        }

        $count--;
        
        $drupal_id      = _dw_campaigns_contact_id_get_user($contact_id);
echo "<!-- HI: $contact_id => $drupal_id -->\n";
        $fake_user      = user_load(array('uid'=>$drupal_id));
        $url            = dw_campaigns_user_get_pcp_url($fake_user, $campaign);
        $image          = _dw_campaigns_get_photo($fake_user, $image_params, 'user-photo', NULL, $image_match);

        $donations      = $counts[$contact_id];
        
        $params = array(
            'contact_id'    => $contact_id,
            'returnFirst'   => 1
        );
        $contact        = _dw_civicrm_contact_get($params);
        
        $rows[]		    = array('name' => $fake_user->displayname, 'drupal_id' => $drupal_id, 'image' => $image, 'url' => $url, 'total' => $total, 'donations' => $donations, 'contact' => $contact, 'campaign_location' => $campaign_location, 'campaign_id' => $campaign->nid);
    }

    return $rows;
}

function _dw_campaigns_all_leaders($count = 3) {

    $row            = array();

    $campaignList   = dw_campaigns_get_campaign_ids();

    $res            = _dw_campaigns_all_leaders_list($campaignList);

    if(is_null($res)) {
        return NULL;
    }

    extract($res);

    $image_match    = '';
    $image_params   = array(
        'w'                 => 45,
        'contribution'      => true,
    );

    $campaigns = array();

    if(count($totals) <= 0 || $totals == 0) {
        return NULL;
    }

    foreach($totals as $idx => $total)
    {
        list($campaign_id, $contact_id)     = split('-', $idx);

        if(dw_campaigns_hide_contact($contact_id)) {
            continue; 
        }
        
        // load the campaign if we havn't seen it before
        if(!isset($campaigns[$campaign_id])) {
            $campaigns[$campaign_id]        = node_load($campaign_id);
        }
        
        $campaign                           = $campaigns[$campaign_id];

        if($count==0)
            break;

        $count--;
        
        $drupal_id      = _dw_campaigns_contact_id_get_user($contact_id);
echo "<!-- HI: $contact_id => $drupal_id -->\n";

        $fake_user      = user_load(array('uid'=>$drupal_id));
        $url            = dw_campaigns_user_get_pcp_url($fake_user, $campaign);
        $image          = _dw_campaigns_get_photo($fake_user, $image_params, 'user-photo', NULL, $image_match);

        $donations      = $counts[$idx];
        
        $params = array(
            'contact_id'    => $contact_id,
            'returnFirst'   => 1
        );
        $contact        = _dw_civicrm_contact_get($params);
        
        $campaign_location  = $campaign->field_dw_campaign_location[0]['value'];
        
        $rows[]		= array('name' => $fake_user->displayname, 'drupal_id' => $drupal_id, 'image' => $image, 'url' => $url, 'total' => $total, 'donations' => $donations, 'contact' => $contact, 'campaign_location' => $campaign_location, 'campaign_id' => $campaign_id);
    }

    return $rows;

}

// LOOK - this doesn't seem to do what we want it to, leaders should probably be an array?
function _dw_campaigns_all_leaders_list($campaignList) {
    static $leaders = NULL;
    
    if(is_null($leaders)) {
        if(count($campaignList) > 0) {

            $totals = array();   
            $counts = array();   

            foreach($campaignList as $campaign_id) {
                $campaign = node_load($campaign_id);

                $currency                       = $campaign->field_dw_currency['0']['value'] ;
                $convert                        = ($currency == 'MULTI') ? TRUE : FALSE;

                $pcps   = _dw_campaigns_get_pcps_for_campaign($campaign_id);
 
                foreach($pcps as $pcp) {
                    $idx    = $campaign_id . '-' . $pcp->contact_id;
    
                    $res            = dw_campaigns_get_contribution_total_for_pcp($pcp, $convert);
                    $totals[$idx]   = $res['total'];
                    $counts[$idx]   = $res['count'];
                }
                
                arsort($totals, SORT_NUMERIC);
                arsort($counts, SORT_NUMERIC);
                
                $leaders                        = array('totals' => $totals, 'counts' => $counts);
            }
        } else {
            $leaders                            = array('totals' => 0, 'counts' => 0);
        }
    }
    return $leaders;
}



/**
 * supply a campaign and a user and it will tell you which position they are ranked based on total donations to PCP
 */
function _dw_campaigns_campaign_find_position_of_user($campaign = NULL, $user) {

    $res                    = _dw_campaigns_campaign_leaders_list($campaign);
    extract($res);

    $our_contact_id         = _dw_campaigns_user_get_contact_id($user);
    
    $high                   = 0;
    $pos                    = 1;
    foreach($totals as $contact_id => $total)
    {
        if($total > $high) {
            $high           = $total;
        }

        if($contact_id == $our_contact_id) {
            break;
        }
        
        $pos++;
    }

    if($high == 0){
        $pos = 1;
    }
    
    return array('position' => $pos, 'total' => $total);
}
/**
 * looks up and returns the
 */
function _dw_campaigns_campaign_get_position($campaign, $pos) {
    $res                    = _dw_campaigns_campaign_leaders_list($campaign);
    extract($res);
    
    foreach($totals as $contact_id => $total) {
    
        $pos--;
        
        if($pos == 0) {
            $drupal_id      = _dw_campaigns_contact_id_get_user($contact_id);
            $fake_user      = user_load(array('uid'=>$drupal_id));
            $url            = dw_campaigns_user_get_pcp_url($fake_user, $campaign);
            
            $donations      = $counts[$contact_id];
            
            $params = array(
                'contact_id'    => $contact_id,
                'returnFirst'   => 1
            );
            
            $contact        = _dw_civicrm_contact_get($params);
            
            $row = array('name' => $fake_user->displayname, 'drupal_id' => $drupal_id, 'contact_id' => $contact_id, 'url' => $url, 'total' => $total);
            return $row;
        }
    }
    
}

function position_to_name($pos) {
    if($pos == 0) {
        return 'N/A';
    }
    
    $str = "$pos";
    
    $t = $pos > 9 ? substr($str,-2,1) : 0;
    $u = substr($str,-1);

    if ($t==1) {
        return $str . 'th';
    } else {
        switch ($u) {
            case 1:
                return $str . 'st';
            case 2:
                return $str . 'nd';
            case 3:
                return $str . 'rd';
            default:
                return $str . 'th';
        }
    }
}

function _dw_campaigns_campaign_find_position_of_amount($campaign = NULL, $amount) {

    $res                    = _dw_campaigns_campaign_leaders_list($campaign);
    extract($res);
    
    $high                   = 0;
    $pos                    = 1;
    foreach($totals as $contact_id => $total)
    {
        if($total > $high) {
            $high           = $total;
        }
        if($total < $amount) {
            break;
       	} 
        $pos++;
    }

    if($high == 0){
        $pos = 1;
    }
    
    return array('position' => $pos, 'total' => $amount);
}


function dw_campaigns_resolve_contact($user) {
    
    // defaults
    $contact = array();
    $contact['email']                   = NULL;
    $contact['phone']                   = NULL;
    $contact['postal_code']             = NULL;
    $contact['state_province_id']       = NULL;
    $contact['country_id']              = NULL;
    $contact['city']                    = NULL;
    $contact['supplemental_address_1']  = NULL;
    $contact['street_address']          = NULL;
    $contact['last_name']               = NULL;
    $contact['first_name']              = NULL;
    
    //load the users civicrm contact if they are logged in to populate default values in form
    if($user->uid > 0) {
        
        if(arg(2) == 'register') {
            drupal_goto('dw');
            return;
        }
            
        $contact = _dw_civicrm_contact_get_from_user($user);
        $contact = get_object_vars($contact); // convert stdObj to an array
    }
    
    return $contact;
}

/**
 * Gets the landing page for a users pcp - personal campaign page
 */
function dw_campaigns_user_get_pcp_url($forUser = NULL, $campaign = NULL, $includeFqd = FALSE) {
	
	if(is_null($forUser)) {
	    global $user;
	    $forUser = $user;
	}

        $path_alias = str_replace('content/', '', $campaign->path);
      
	if(is_null($campaign)) {
	    $return_data = "/dw/users/" . $forUser->name;
	} else {
	    $return_data = "/dw/users/" . $forUser->name . "/" . $path_alias;
	}

	$return_data = preg_replace("_//*_", '/', $return_data);

	if($includeFqd) {
            $cur_host    = $_SERVER['HTTP_HOST'];
	    $return_data = "http://" . $cur_host . $return_data;
	}

	return $return_data;
}
/**
 * takes '2011-kentucky-derby' and converts to node id (4)
 */
function dw_campaigns_campaign_get_id_from_url($campaignPath) {
    $campaignId         = NULL;
    
    // I know some people don't use a prefix (fpwr did not) - or they may use more complicated paths
    $path               = drupal_lookup_path('source', $campaignPath, 'en');
    
    if(!$path) {
        // we use content/whatever as our prefix
        $path           = drupal_lookup_path('source', 'content/' . $campaignPath, 'en');
    }
    
    if($path) {
        list($junk, $campaignId)  = explode('/', $path);
    }

    return $campaignId;

}

/**
 * takes '4' and returns $prefix/2011-kentucky-derby
 */
function dw_campaigns_get_campaign_path($campaignId, $prefix = '/dw/campaign', $suffix = '') {
    $path_alias = drupal_get_path_alias("node/" . $campaignId);

    // remove the content/ prefix
    $path_alias = str_replace('content/', '', $path_alias);
    
    $path = $prefix . '/' . $path_alias . '/' . $suffix;
    // remove the dupes
    $path = preg_replace('/\/\/*/', '/', $path);
    // remove a trailing /
    $path = preg_replace('/\/$/', '', $path);
    return $path;
}

/**
 * pass either a campaign id (nid) or a campaign object
 */

function dw_campaigns_get_donate_url($user, $campaign) {
    $campaign = (is_object($campaign)) ? $campaign : node_load($campaign);
    
    $prefix = 'dw/users/' . $user->name;
    
    return dw_campaigns_get_campaign_path($campaign->nid, $prefix, 'donate');
}


function dw_campaigns_campaign_search_pcp_search($campaign, $in_query) {
    $matches            = array();

    $params             = array();
    $params['search']   = $in_query;
    $civi_contacts      = _dw_civicrm_pcp_search_owner($params);
    if($campaign === '0') {
        $pcps           =  _dw_campaigns_get_all_pcps_keyed_by_contact_ids();
    } else {
        $pcps           = _dw_campaigns_get_pcps_for_campaign_keyed_by_contact_ids($campaign);
    }

    foreach($civi_contacts as $contact_id => $civi_contact)
    {
        if(isset($pcps[$contact_id]))
        {
            $_user                  = user_load(_dw_campaigns_contact_id_get_user($contact_id));
            $civi_contact->user     = $_user;
            
            $matches[$contact_id]   = $civi_contact;
        }
    }

    return $matches;
}

function dw_campaigns_campaign_member_search($campaignPath, $in_query = NULL) {

    $return         = '<h2>Search Results</h2>';
  
    $mode           = dw_campaigns_get_selected_type();
 
    if($campaignPath == 'current')  {
         $campaign = '0';
    } else {   
        $campaignId     = dw_campaigns_campaign_get_id_from_url($campaignPath);
        $campaign       = node_load($campaignId);
    
        dw_campaigns_set_selected_campaign($campaign);
    
        if($campaign->nid == 0) {
    	    drupal_goto("dw");
    	    return;
        }
    }

    if(is_null($in_query)) {
        $in_query	=	isset($_REQUEST['query'])?$_REQUEST['query']:'';
    }

    if($in_query === 'Name' || $in_query == 'search for a fundraiser')
    {
    	$in_query = '';
    }

    if(strlen($in_query)<2) {
        return $return . 'Minimum search length is 2 characters';        
    }

    $results = dw_campaigns_campaign_search_pcp_search($campaign, $in_query);

    if($mode == 'walking') {
        $headers    = array('','','', '');
    } else {
        $headers    = array('','','');
    }
    $rows       = array();
    
    foreach($results as $result) {
        $prefix = '/dw/users/' . $result->user->name;
        $dummy  = '';

        // this means its for a specific campaign
        if($campaignPath != 'current') {
            $path   = dw_campaigns_get_campaign_path($campaign->nid, $prefix, '');

            $row    = array();
            $row[]  = '<img src="' . _dw_campaigns_get_photo($result->user, array('w' => 50), 'user-photo', NULL, $dummy) . '" />';
            $row[]  = $result->display_name . ' (' . $result->pcp_title . ')';
            if($mode == 'walking') {
                $row[]  = $campaign->field_dw_campaign_location[0]['value'];
            }
            $row[]  = '<a href="' . $path . '" class="btn">Visit Page</a>';
            $rows[] = $row;

	} else {
	    $pcp_data  = _dw_campaigns_get_pcp_by_drupal_id($result->user);
            $campaigns = dw_campaigns_get_campaigns(true); // these come back indexed by contribution page

            foreach($pcp_data as $pcp_id => $pcp) {

                $contribution_page_id = $pcp->contribution_page_id;

		if(isset($campaigns[$contribution_page_id])) {
                    $campaign = $campaigns[$contribution_page_id];
                } else {
                    continue;
                }
                
                $path   = dw_campaigns_get_campaign_path($campaign->nid, $prefix, '');
    
                $row    = array();
                $row[]  = '<img src="' . _dw_campaigns_get_photo($result->user, array('w' => 50), 'user-photo', NULL, $dummy) . '" />';
                $row[]  = $result->display_name . ' (' . $result->pcp_title . ')';

                if($mode == 'walking') {
                    $row[]  = $campaign->field_dw_campaign_location[0]['value'];
                }

                $row[]  = '<a href="' . $path . '" class="btn">Visit Page</a>';
                $rows[] = $row;

            }
        }
    }

    if(count($rows) == 0) {
        return $return . 'No matches found for (' . htmlentities($in_query) . ')';   
    }
    
    return $return . theme('table', $headers, $rows);
}


function _dw_campaigns_get_pcps_for_campaign_keyed_by_contact_ids($campaign) {

    $contacts = array();
    
    $res = _dw_campaigns_get_pcps_for_campaign($campaign);

    foreach($res as $pcp) {
        $contacts[$pcp->contact_id] = $pcp;
    }
    
    return $contacts;
}

function _dw_campaigns_get_all_pcps_keyed_by_contact_ids() {

    $contacts = array();
   
    $campaigns = dw_campaigns_get_campaigns(); 
    if(count($campaigns) > 0) {
        foreach($campaigns as $campaign) {
            $res = _dw_campaigns_get_pcps_for_campaign($campaign);
        
            foreach($res as $pcp) {
                $contacts[$pcp->contact_id] = $pcp;
            }
        } 
    }
    return $contacts;
}


function dw_campaigns_pcp_get_supporters($pcp) {

    $res    = dw_campaigns_get_contributions_for_pcp($pcp);

    return $res;
}

function dw_campaigns_get_default_campaign() {
    return variable_get('dw_campaigns_default_campaign', '4');
}

function dw_campaigns_force_decimal($in, $currency = 'USD', $always_show_usd = false) {
    $prefix     = '$';
    $extra      = '';

    switch($currency) {
        case 'AUD':
            $prefix = 'AU$';
            break;
        case 'CAD':
            $prefix = 'CA$';
            break;
        case 'EUR':
            $prefix = 'EUR ';
            break;
        case 'GBP':
            $prefix = 'GBP ';
            break;
        case 'NZD':
            $prefix = 'NZ$ ';
            break;

    }

    if($always_show_usd) {
	$in_usd = dw_campaigns_convert_to_usd($currency, $in);

        $usd     = dw_campaigns_force_decimal($in_usd, 'USD');
        $extra   = " ($usd) ";
    }

    $fmt         = sprintf("%s%s%s", $prefix, number_format($in, 2), $extra);

    return $fmt;
}

function dw_campaigns_convert_to_usd($currency, $in) {
    
// prices according to xe.com  Live rates at 2011.04.01 18:47:00 UTC
    switch($currency) {
        case 'AUD':
            $in = $in * 1.03861;
            break;
        case 'CAD':
            $in = $in * 1.03643;
            break;
        case 'EUR':
            $in = $in * 1.42386;
            break;
        case 'GBP':
            $in = $in * 1.61264;
            break;
        case 'NZD':
            $in = $in * 0.8554;
            break;

    }

    return $in;
}

function dw_campaigns_force_k($in) {
    if($in < 1000) {
        return dw_campaigns_force_decimal($in);
    }

    $k  = $in/1000;

    return floor($k) . 'K';
}

function dw_campaigns_build_location_addr($address) {
    $res = $address['street'] . ' ' . $address['city'] . ', ' . $address['province'] . ' ' . $address['postal_code'];
    return $res;
}

// completely taken from Taylor
function dw_campaigns_get_location_for_address($address) {
	$cacheKey	= "geo-" . md5($address);
	$location 	= cache_get($cacheKey);

	if($location == 0) {
		
		$results 	= json_decode(file_get_contents("http://maps.google.com/maps/api/geocode/json?address=" . urlencode($address) . "&sensor=false"));
		$location 	= (array) $results->results[0]->geometry->location;
		
		cache_set($cacheKey, $location);
		
	} else {
		$location = $location->data;
	}
	
	return $location;
}


function dw_campaigns_get_visitor_location() {
    
    //check cache
    $ip     = $_SERVER["REMOTE_ADDR"];
    $tags   = cache_get('geo-for-' . $ip);
          
    if(!$tags) {
        $tags   = get_meta_tags('http://www.geobytes.com/IpLocator.htm?GetLocation&template=php3.txt&IpAddress=' . $ip);
    	cache_set('geo-for-' . $ip, $tags);
    } else {
    	$tags	= $tags->data;
    }
    
    //the ip location service returns a string of false if it's not found
    if($tags['known'] == 'false') {
    	//if not found, use center of unitded states
    	$tags['known']      = false;
    	
    	$results    = dw_campaigns_get_location_for_address("United States");
    	
    	$tags['latitude']   = $results->lat;
    	$tags['longitude']  = $results->lng;
    	
    } else {
    	$tags['known']      = true;
    }
    
    return $tags;
}


  
//function calculate_distance  (seen multiple places with google search)
// I copied this from http://www.sudhirhub.com/2010/05/find-distance-between-two-cities-using.html
function calculate_distance($lat1, $long1, $lat2, $long2, $unit = 'M') {
/*
echo "1 - " . $lat1 ."<br>";
echo "2 - " . $long1 ."<br>";
echo "3 - " . $lat2 ."<br>";
echo "4 - " . $long2 ."<br>";
*/
    $theta = $long1 - $long2;  
    $dist = sin(deg2rad($lat1)) * sin(deg2rad($lat2)) +  cos(deg2rad($lat1)) * cos(deg2rad($lat2)) * cos(deg2rad($theta));  
    $dist = acos($dist);  
    $dist = rad2deg($dist);  
    $miles = $dist * 60 * 1.1515;  
    $unit = strtoupper($unit);  
    if ($unit == "K") {  
        return ($miles * 1.609344);  
    }  
    else if ($unit == "N") {  
         return ($miles * 0.8684);  
    }  
    else {
        return $miles;  
    }  
}

function dw_campaigns_get_close_campaigns($in_query = NULL) {

    if(is_null($in_query)) {
        $in_query	=	isset($_REQUEST['query'])?$_REQUEST['query']:'';
    }

    if(empty($in_query)) {
        $userGeo    = dw_campaigns_get_visitor_location();
    } else {
    	$results    = dw_campaigns_get_location_for_address($in_query);

    	$userGeo['latitude']   = $results['lat'];
    	$userGeo['longitude']  = $results['lng'];
    }

    $unit = variable_get('dw_campaigns_walking_distance_unit', 'M');
    $campaigns = dw_campaigns_get_campaigns();
    
    $dist_camp = array();
    foreach($campaigns as $campaign) {
        $address            = dw_campaigns_build_location_addr($campaign->field_dw_address[0]);
        $campLocation       = dw_campaigns_get_location_for_address($address);

	// don't show locations which don't have a lat/long
	if(!isset($campLocation['lat'])) {
            continue;
        }

        $miles              = calculate_distance($userGeo['latitude'], $userGeo['longitude'], $campLocation['lat'], $campLocation['lng'], $unit);
        $key                = $miles . '-' . $campaign->nid;
        $dist_camp[$key]    = $campaign;
    }
    ksort($dist_camp, SORT_NUMERIC);
    
    return $dist_camp;
}

function dw_campaigns_get_body_class() {
    return dw_campaigns_make_body_class();
}

function dw_campaigns_get_class_body() {
    return dw_campaigns_make_body_class();
}

function dw_campaigns_make_body_class() {
    if(arg(4) == 'donate') {
        return 'donation-page';
    }
    if(arg(4) == 'donated') {
        return 'donate-complete';   
    }
    if(arg(4) == 'supporters') {
        return 'donors-page';   
    }
    if(arg(3) == 'leaderboard') {
        return 'leaderboard';
    }
    // this page is almost identical to the homepage, so let it have its styling
    if(substr($_GET['q'], 0, 19) == 'dw/walking/location') {
        return 'dw-walking walking-location';   
    }

    $extra = '';
    if(arg(1) == 'users') {
        $extra = ' dw-users';
    }   

    if(arg(1) == 'user') {
        $extra = ' dw-user';
    }

    if($_GET['q'] == 'dw/walking') {
        $extra = ' walking-home';
    } 

    if(isset($_REQUEST['ajax'])) {
       $extra .= ' ajax';
    }

    $page = $_GET['q'];
    $page = strtolower($page);
    $page = preg_replace("_/_", '-', $page);
    $page = preg_replace("/[^a-z0-9\-]/", '', $page);
    
    return $page . $extra;;
}

function dw_campaigns_get_all_campaign_location() {
    static $list = NULL;
    
    if(is_null($list)) {
        $campaigns  = dw_campaigns_get_campaigns();
        $list       = array();
        if(count($campaigns) > 0 ) {
            foreach($campaigns as $campaign) {
                $nid        = $campaign->nid;

                $location   = $campaign->field_dw_campaign_location[0]['value'];
		if(dw_campaigns_hide_campaign($campaign) !== FALSE) {
                    // skip
                } else {
                    $list[$nid] = $location;
                }
            }
        } 
        asort($list, SORT_STRING);
    }
    return $list;
}

function dw_campaigns_get_user_pcp_details($user) {   
    $mode_type  = dw_campaigns_get_selected_type();
    
    $campaignId = NULL;
    $campaign   = NULL;
    $pcp        = NULL;
    $rawurl     = NULL;
    
    if($mode_type == 'walking') {	
        $pcp = dw_campaigns_user_get_pcp_for_walking($user);

        if(!is_null($pcp)) {    
            $contribution_page_id   = $pcp->contribution_page_id;
    
            $campaigns  = dw_campaigns_get_campaigns(TRUE);
            $campaign   = $campaigns[$contribution_page_id];
            $campaignId = $campaign->nid;
        }
    } else {
        $campaignId = dw_campaigns_get_default_campaign();
        
        $campaign   = node_load($campaignId);
        $pcp        = dw_campaigns_user_get_pcp_for_campaign($user, $campaign);
    }
    
    if(!is_null($pcp->id)) {
        $rawurl = dw_campaigns_user_get_pcp_url($user, $campaign, TRUE);
    }
    
    return array('campaign_id' => $campaignId, 'campaign' => $campaign, 'pcp' => $pcp, 'url' => $rawurl);
    
}

function dw_campaigns_hide_campaign($campaign) {

    $campaign = is_object($campaign) ? $campaign : node_load($campaign);

    $location = isset($campaign->field_dw_campaign_location[0]['value']) ? $campaign->field_dw_campaign_location[0]['value'] : NULL;

    if(strstr($location, 'DONOTDISPLAY') !== FALSE) {
        return TRUE;
    }
    
    return FALSE;
}

function dw_campaigns_hide_contact($contactId) {

    $params = array(
        'contact_id'    => $contactId,
        'returnFirst'   => 1
    );
    
    $contact        = _dw_civicrm_contact_get($params);
    if($contact->last_name == 'GeneralDonation') {
        return TRUE;
    }

    return FALSE;
}

function dw_campaign_get_event_participants() {
   $campaigns = dw_campaign_get_event_participants_campaigns();

   foreach($campaigns as $campaignId) {
       $data[$campaignId] = dw_campaign_get_event_participants_by_campaign($campaignId);
   }

   return $data;
}

function dw_campaign_get_event_participants_campaigns() {
    $data = array();

    $result = db_query("select campaign_id from `dw_campaigns_event_participants`");

    while($row = db_fetch_object($result)) {
        $data[] = $row->campaign_id;
    }

    return $data;
}

function dw_campaign_get_event_participants_by_campaign($campaignId) {

    $result = db_query("select * from `dw_campaigns_event_participants` where campaign_id = '%s'", $campaignId);

    $data = array();

    while($row = db_fetch_object($result)) {
        if($row->pcp_id != 0) {
            $pcp = dw_campaigns_user_get_pcp_by_id($row->pcp_id);
            $contactId = $pcp->contact_id;
        } else {
            $contactId = $row->contact_id;
        }

        $params = array(
            'contact_id'    => $contactId,
            'returnFirst'   => 1
        );
        $contact = _dw_civicrm_contact_get($params);

        $data[$contactId] = array('contact' => $contact, 'row' => $row);
    }

// we were missing a lot of data, so lets look up some more people

    $pcps = _dw_campaigns_get_pcps_for_campaign($campaignId);

    foreach($pcps as $pcp) {
        $contactId = $pcp->contact_id;
        if(!isset($data[$contactId])) {
            $params = array(
                'contact_id'    => $contactId,
                'returnFirst'   => 1
            );

            $contact               = _dw_civicrm_contact_get($params);

            $dummy      = new stdClass;
            $dummy->id  = $pcp->contribution_page_id;
            $campaign   = dw_campaigns_get_campaign_from_contribution_page($dummy);

            $row                   = new stdClass;

            $row->campaign_id      = $campaign->nid;
            $row->participant_type = 'adult_unknown';
            $row->pcp_id           = $pcp->id;
            $row->participants     = 1;
            // we currently don't set contact id if we have a pcp, NOTE - contact_id can get out of sync with dedupe merges!!!
            $row->contact_id       = 0; // $pcp->contact_id; 


            $data[$contactId] = array('contact' => $contact, 'row' => $row);
        } 
    }


    return $data;
}

function dw_campaigns_build_receipt_merge($campaign, $pcp_id, $contact_id, $contribution) {

    $mergeObjects   = dw_campaigns_get_merge_object($campaign, $pcp_id, $contact_id);
    // extra fields, fixups
    $mergeObjects['contribution']               = $contribution;
    $mergeObjects['contribution']->total_amount = dw_campaigns_force_decimal($mergeObjects['contribution']->total_amount, $campaign->field_dw_currency['0']['value']);

    $receive_date                               = str_replace("-", "", $mergeObjects['contribution']->receive_date);
    $temp_receive_string                        = substr($receive_date, 0, 4) . '/' . substr($receive_date, 4, 2) . '/' . substr($receive_date, 6, 2);

    $mergeObjects['contribution']->receive_date = date("M d, Y", strtotime($temp_receive_string));


    // this is a kludgy work around, I split the merge object creation so we can use this more places
    $pcp_contact    = $mergeObjects['pcp_contact'];

    $from = sprintf("%s <%s>", $campaign->field_dw_receipt_from_name[0]['value'], $campaign->field_dw_receipt_from_email[0]['value']);
    $mergeObjects['campaign_obj'] = $campaign;

    return $mergeObjects;
}



function dw_campaign_receipt_report($arg) {
    $merge_data = dw_campaigns_build_receipt_data($arg); 
    $data=theme('dw_campaigns_receipt_report', $merge_data);
    return $data;
}

function dw_campaigns_build_receipt_data($arg) {
    // lookup regular contributions
    $params                             = array();
    $params['contribution_id']          = $arg;
    $params['rowCount']                 = 999999;
    $contributions                      = _dw_civicrm_contribution_get($params);

    foreach($contributions as $contribution) {

    }

    $softcontributions  = _dw_civicrm_contributionsoft_get($params);
    foreach($softcontributions as $softcontribution) {

    }

    $pcp = dw_campaigns_user_get_pcp_by_id($softcontribution->pcp_id);

    $dummy      = new stdClass;
    $dummy->id  = $pcp->contribution_page_id;
    $campaign   = dw_campaigns_get_campaign_from_contribution_page($dummy);

    $merge_data = dw_campaigns_build_receipt_merge($campaign, $softcontribution->pcp_id, $softcontribution->contact_id, $contribution);

    return $merge_data;
}

function dw_campaign_offline_report($arg, $csv = FALSE) {
    $campaign = node_load($arg);
    $pcps = _dw_campaigns_get_pcps_for_campaign($campaign);


    $printed = 0;

    echo "<table>";

    $contributions = NULL;

    foreach($pcps as $id => $pcp) {

        $params             = array();
        $params['pcp_id']   = $pcp->id;
        $params['rowCount'] = 999999;

        $softcontributions  = _dw_civicrm_contributionsoft_get($params);

        if(is_null($contributions)) {
            $params                             = array();
            $params['contribution_page_id']     = $pcp->contribution_page_id;
            $params['rowCount']                 = 999999;
            $params['contribution_status_id']   = 2; // pending only
            $contributions                      = _dw_civicrm_contribution_get($params);
        }

        $params = array(
            'contact_id'    => $pcp->contact_id,
            'returnFirst'   => 1
        );
        $pcp_contact        = _dw_civicrm_contact_get($params);

        foreach($softcontributions as $id => $softcontribution) {
            // this should never happen
            if($softcontribution->pcp_id != $pcp->id) {
                continue;  
            }

            $key = '_' . $softcontribution->contribution_id;
            $contribution = $contributions->$key;

            // since we only look up contributions which are "offline" we need to skip a lot
            if(is_null($contribution)) {
                continue;
            }

            $maxtitlelength = 20;

            $title = $pcp->title;
            if(strlen($title) > $maxtitlelength) {
                $title = substr($pcp->title, 0, $maxtitlelength) . '...';
            }

            $params = array(
                'contact_id'    => $contribution->contact_id,
                'returnFirst'   => 1
            );
            $donor_contact      = _dw_civicrm_contact_get($params);
//var_dump($donor_contact);
            $temp         = explode(",", $contribution->sort_name);
            $parts = array(
                'PCP Civi Id'             => $pcp_contact->contact_id,
                'PCP Owner'               => $pcp_contact->display_name,
                'PCP Title'               => $title,              
                'Donor Civi Id'           => $contribution->contact_id,
                'Donor Last'              => $temp[0],
                'Donor First'             => $temp[1],
                'Donor Email'             => $donor_contact->email,
                'Donor Address'           => $donor_contact->street_address,
                'Donor City'              => $donor_contact->city,
                'Donor State/Province'    => $donor_contact->state_province_name,
                'Donor Country'           => $donor_contact->country,
                'Donor Postal Code'       => $donor_contact->postal_code,
                'Amount'                  => $contribution->total_amount,
                'Payment Instrument'      => str_replace('Credit Card', 'Cash', $contribution->payment_instrument),
                'Check #'                 => $contribution->check_number,

            );
            if($printed == 0) { 
                $keys = array_keys($parts);
                echo '<tr><th>' . implode('</th><th>', $keys) . '</th></tr>';   
                $printed = 1;
            }

            $keys = array_values($parts);
            echo '<tr><td>' . implode('</td><td>', $keys) . '</td></tr>';   
        }
    }
    echo '</table>';
}

function dw_campaign_pcp_report($arg = 'ALL', $csv = FALSE) {
//$csv = true;
    //dw_campaigns_cache_dirty('PCP-get', '');
    $arg = trim($arg);

    $file = drupal_get_path('module','dw_campaigns') . "/civi_cache/report-$arg-" . time() . ".csv";
    $fp = fopen($file, "w");

    $printed = 0; 

    ob_start();

    $master_list = dw_campaigns_get_all_campaign_location();

    if(strcasecmp($arg, 'ALL') == 0) {
        $list = $master_list; 
    } else {
        $list[$arg] = $master_list[$arg];
    }

    echo "<table>";
    foreach($list as $campaign_id => $location) {
        $data = dw_campaign_get_event_participants_by_campaign($campaign_id);
        $campaign = node_load($campaign_id);
        foreach($data as $row) {
            $total_fee = 0;
            $total_donation = 0;
        
            if($row['row']->pcp_id > 0) {
        
                $dummy          = new stdClass;
                $dummy->id      = $row['row']->pcp_id;
                $totals         = dw_campaigns_get_contribution_total_for_pcp($dummy);
                $total_count    = $totals['count'];
                $total_donation = $totals['total'];
                $total_offline  = $totals['offline'];
                $total_fee      = 0;
            } else {
                $params = array();
                $params['contact_id']   = $row['row']->contact_id;
                $params['rowCount']     = 999999;
        
                $contributions = _dw_civicrm_contribution_get($params);

                $total_count    = 0;
                $total_donation = 0;
                $total_fee      = 0;
                $total_offline  = 0;

                foreach($contributions as $donation) {
                    $total_fee      += $donation->non_deductible_amount;
                    $total_donation += ($donation->total_amount - $donation->non_deductible_amount);
                }

            }
       
            $parts = array(
                'civi_id'          => $row['contact']->contact_id,
                'first_name'       => $row['contact']->first_name,
                'last_name'        => $row['contact']->last_name,
                'display_name'     => $row['contact']->display_name,
                'email'            => $row['contact']->email,
                'phone'            => $row['contact']->phone,
                'location'         => $location,
                'currency'         => $campaign->field_dw_currency[0]['value'],
                'total_donations'  => $total_donation,
                'total_offline'    => $total_offline,
                'total_fees'       => $total_fee,
                'num_donations'    => $total_count,
                'participant_type' => $row['row']->participant_type,
                'participant_num'  => $row['row']->participants,
            ); 
    
            if($row['contact']->last_name == 'GeneralDonation') {
                $name    = 'Total non pcp donations and fees';
                $email   = '';
                continue;
            }
        
            if($row['row']->pcp_id == 0 && $row['row']->contact_id == 0) {
                echo "error, both pcpid and contactid are 0!";
                continue;
            }
       
            if($printed == 0) { 
                $keys = array_keys($parts);
                array_walk($keys, 'dw_campaigns_report_headers');
                fputcsv($fp, $keys);
                echo '<tr><th>' . implode('</th><th>', $keys) . '</th></tr>';   
                $printed = 1;
            }
    
            $keys = array_values($parts);
            fputcsv($fp, $keys);
            echo '<tr><td>' . implode('</td><td>', $keys) . '</td></tr>';   
        }
        //flush();
    }
    echo "</table>";
    
    $contents = ob_get_contents();
    ob_end_clean();
    fclose($fp);

    if(!$csv) {
        echo $contents;
    } else {
        $date = date("Y-m-d_His");
        $outfile = 'report-' . $arg . '-' . $date . '.csv';
        $fsize   = filesize($file);

        header("Pragma: public"); // required
        header("Expires: 0");
        header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
        header("Cache-Control: private",false); // required for certain browsers
        header("Content-Type: application/force-download"); 
        header("Content-Disposition: attachment; filename=\"$outfile\"");
        header("Content-Transfer-Encoding: binary");
        header("Content-Length: $fsize");
        header("Content-type: text/csv");   
        echo file_get_contents($file);
    }
}

function dw_campaigns_report_headers(&$value, $key) {
    $value = ucfirst(str_replace("_", ' ', $value));
}

?>
