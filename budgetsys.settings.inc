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
  // Setting for allowing a span of 2 years for a fiscal year
  $form['budgetsys_multiyear_fiscal_year'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('budgetsys_multiyear_fiscal_year', 0),
    '#title' => t('Multiyear Fiscal Year?'),
    '#description' => t('Check this box if your organization uses a multiyear value for fiscal year. i.e. 2012-2013'),
  );
  // The current budgeting year
  $form['budgetsys_current_fiscal_year'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('budgetsys_current_fiscal_year', ''),
    '#title' => t('Current Budgeting Year'),
    '#description' => t('Please enter the year for which you are assembling a budget for. <br>If you are assembling a budget enter the current fiscal year.'),
    '#size' => 9,
  ); 
  // Allowed Fiscal Years
  $form['budgetsys_allowed_fiscal_years'] = array(
    '#type' => 'textarea',
    '#default_value' => variable_get('budgetsys_allowed_fiscal_years',''),
    '#title' => t('Allowed Fiscal Years'),
    '#required' => TRUE,
    '#description' => t('Please enter the allowed fiscal years, separated by comma followed by a space (, ).<br>
    The allowed fiscal yaers will appear when creating a new Budget Value'),
  );  
  return system_settings_form($form);
}
