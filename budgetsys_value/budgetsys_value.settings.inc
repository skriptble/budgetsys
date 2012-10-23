<?php

/**
 * Form builder; Build the Budget Value settings form.
 */
function budgetsys_value_configuration_form($form, &$form_state) {
  // Select the final value type that is displayed for past years
  $form['budgetsys_value_final_type'] = array(
    '#type' => 'select',
    '#default_value' => variable_get('budgetsys_value_final_type',''),
    '#title' => t('Budget Final Value Type'),
    '#required' => TRUE,
    '#options' => budgetsys_value_types_array(), 
    '#description' => t('Please select the budget value type that will be used to display prior year values in the main budget<br>and on line items (if enabled)'),
  );
  
  return system_settings_form($form);
}
