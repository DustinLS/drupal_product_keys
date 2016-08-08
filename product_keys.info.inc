<?php

	/**
	* @file
	* Provides metadata for the product entity.
	*/

	/**
	* Implements hook_entity_property_info().
	*/
	function product_keys_entity_property_info() {
		$info = array();

		// Add meta-data about the basic commerce_product properties.
		$properties = &$info['commerce_product']['properties'];

		$properties['product_key'] = array(
			'label' => t('Product Key'),
			'description' => t('The product key assigned to a product.'),
			'type' => 'text',
			'schema field' => 'product_key'
		);

		$properties['product_keys'] = array(
			'label' => t('Product Keys'),
			'description' => t('The number of product keys in stock.'),
			'type' => 'integer',
			'setter callback' => 'entity_property_verbatim_set',
			'schema field' => 'product_keys'
		);
	}

	/**
	* Implements hook_entity_property_info_alter() on top of the Product module.
	*/
	function product_keys_entity_property_info_alter(&$info) {
		if (!empty($info['commerce_product']['bundles'])) {
			// Move the default price property to the product by default; as it is a
			// required default field, this makes dealing with it more convenient.
			$properties = array();

			foreach ($info['commerce_product']['bundles'] as $bundle => $bundle_info) {
				$bundle_info += array('properties' => array());
				$properties += $bundle_info['properties'];
			}

			if (!empty($properties['commerce_price'])) {
				$info['commerce_product']['properties']['commerce_price'] = $properties['commerce_price'];
			}
		}
	}