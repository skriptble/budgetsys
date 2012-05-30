<?php

/**
 * Form builder; Build the Budget Values settings form.
 */
function budget_value_configuration_form($form, &$form_state) {
  // Selectable currency symbol used when displaying monetary values
  $form['budgetsys_budget_value_currency_symbol'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('budgetsys_budget_value_currency_symbol','$'),
    '#title' => t('Currency Symbol'),
    '#size' => 5,
    '#maxlength' => 10,
    '#required' => TRUE,
    '#description' => t('This is the symbol displayed before any monetary value. i.e. the $ symbol in $1,000'),
  );
  
  return system_settings_form($form);
}
