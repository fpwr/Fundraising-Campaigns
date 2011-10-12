<?php

	$node_id = variable_get('dw_campaigns_signup_words_node', '');

	if(is_numeric($node_id)) {
		$node   = node_load($node_id);
	}
	if($node->nid == $node_id) {
		echo '<h2>' . $node->title . '</h2>';
		echo $node->body;
	}