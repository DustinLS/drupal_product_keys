<?php

	/**
	* @file
	* Token hooks for the product keys manager module.
	*/

	/**
	* Implements hook_token_info().
	*/
	function product_keys_token_info() {
		$order['product-key'] = array(
			'name' => t('Product key'),
			'description' => t('The product key.'),
		);

		$product['product-key'] = array(
			'name' => t('Product key'),
			'description' => t('The product key.'),
		);

		$product['product-keys'] = array(
			'name' => t('Product keys'),
			'description' => t('The number of product keys in stock.'),
		);

		return array(
			'tokens' => array(
				'commerce-order' => $order,
				'commerce-product' => $product
			),
		);
	}

	/**
	* Implements hook_token_info_alter().
	*/
	function product_keys_token_info_alter(&$info) {
		$info['tokens']['commerce-order']['product-key'] = array(
			'name' => t('Product key'),
			'description' => t('The product key.'),
		);

		$info['tokens']['commerce-product']['product-key'] = array(
			'name' => t('Product key'),
			'description' => t('The product key.'),
		);

		$info['tokens']['commerce-order']['product-keys'] = array(
			'name' => t('Product keys'),
			'description' => t('The number of product keys in stock.'),
		);
	}

	/**
	* Implements hook_tokens().
	*/
	function product_keys_tokens($type, $tokens, $data = array(), $options = array()) {
		$values = array();

		if ($type == 'commerce-order' && !empty($data['commerce-order']))  {
			$order = $data['commerce-order'];

			if (isset($tokens['product-key']) || isset($tokens['product-title:0:0'])) {
				$index = -1;
				$tokens['orders'] = array();

				if(!isset($tokens['product-key'])) {
					$tokens['product-key'] = "";
				}

				// Get customer id from order
				$result = db_select('commerce_order', 'n')->fields('n')->condition('order_id', $order->order_id, '=')->execute()->fetchAssoc();
				$tokens['order-uid'] = $result['uid'];

				// Get line item, sku, and quantity of order
				$query = db_select('commerce_line_item', 'n')->fields('n')->condition('order_id', $order->order_id, '=')->execute();

				while($result = $query->fetchAssoc()) {
					$index++;
					$sku = $result['line_item_label'];
					$product_id = $result['line_item_id'];
					$qty = $result['quantity'];
					$tokens['orders']["$sku"] = $result;
					$tokens['orders']["$sku"]["uid"] = $tokens['order-uid'];
					$tokens['orders']["$sku"]["sku"] = $sku;

					$product_query = db_select('commerce_product', 'n')->fields('n')->condition('sku', $sku, '=')->execute();
					$product_assoc = $product_query->fetchAssoc();
					$product_title = $product_assoc['title'];

					for($i = 0; $i < $qty; $i++) {
						$tokens["orders"]["$sku"]["license"] = product_keys_assign($tokens['orders']["$sku"], $i);
						$tokens["product-key:$index:$i"] = product_key_decrypt($tokens["orders"]["$sku"]["license"]['license_key']);

						$values["[product-key]"] = $tokens["product-key"]  || "N/A";
						$values["[commerce-order:product-title]"] = $product_title;
						$values["[commerce-order:product-key]"] = $tokens["product-key"]  || "N/A";
						$values["[commerce-order:product-title:$index:$i]"] = $product_title;
						$values["[commerce-order:product-key:$index:$i]"] = $tokens["product-key:$index:$i"];
					}

					$values["[commerce-product:product-keys]"] = product_keys_stock($sku);
				}
			}

			if (!empty($data['commerce-product']))  {
				$product = $data['commerce-product'];
				$values["[commerce-product:product-keys]"] = product_keys_stock($product->sku);
			}

			if (isset($tokens['product-key'])) {
				$original = $tokens['product-key'];

				// $values[$original] = "{" . $original . ": " . $tokens['product-key'] . "}";
				if (empty($values[$original])) {
					$values[$original] = "N/A";
				}
			}
		}

		return $values;
	}