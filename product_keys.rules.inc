<?php

	/**
	* Implements hook_rules_action_info().
	*/
	function product_keys_rules_action_info() {
		$actions = array();

		$actions['product_keys_assign'] = array(
			'label' => t('Assign product keys to order'),
			'parameter' => array(
				'commerce_order' => array(
					'type' => 'commerce_order',
					'label' => t('Reference Order'),
				),
			),
			'group' => t('Product Keys'),
			'callbacks' => array(
				'execute' => 'product_keys_rules_assign_keys',
			),
		);

		return $actions;
	}

	/**
	* Rules action: updates an order's status to the default status of the given
	*   order state.
	*/
	function product_keys_rules_assign_keys($order, $name) {
		// Get customer id from order
		$result = db_select('commerce_order', 'n')->fields('n')->condition('order_id', $order->order_id, '=')->execute()->fetchAssoc();
		$tokens['order-uid'] = $result['uid'];

		// Get line item, sku, and quantity of order
		$query = db_select('commerce_line_item', 'n')->fields('n')->condition('order_id', $order->order_id, '=')->execute();

		while($result = $query->fetchAssoc()) {
			$sku = $result['line_item_label'];
			$product_id = $result['line_item_id'];
			$qty = $result['quantity'];
			$tokens['orders']["$sku"] = $result;
			$tokens['orders']["$sku"]["uid"] = $tokens['order-uid'];
			$tokens['orders']["$sku"]["sku"] = $sku;

			for($i = 0; $i < $qty; $i++) {
				product_keys_assign($tokens['orders']["$sku"], $i);
			}
		}
	}

	/**
	* Rules action: updates an order's status using the Order API.
	*/
	function product_keys_rules_update_status($order, $name) {
		//commerce_order_status_update($order, $name, FALSE, NULL, t('Order status updated via Rules.'));
	}