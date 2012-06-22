<?php

/**
 * Form builder; Build the Budget Values settings form.
 */
function budget_value_configuration_form($form, &$form_state) {
  // The name of the budget
  $form['budgetsys_budget_name'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('budgetsys_budget_name', ''),
    '#title' => t('Budget Name'),
    '#description' => t('Please enter the name of the budget.'),
  ); 
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
  // Number of proior years to display 
  $form['budgetsys_line_num_prior_years'] = array(
    '#type' => 'select',
    '#default_value' => variable_get('budgetsys_line_num_prior_years', '3'),
    '#options' => array(
      1 => t('1'),
      2 => t('2'),
      3 => t('3'),
      4 => t('4'),
    ),
    '#title' => t('Number of Prior Years'),
    '#description' => t('Please select the number of prior years you would like to show when viewing line items. 
    <br>If you are budgeting for 2014 and select 3 the years 2013,2012, and 2011 will display.'),
  ); 
  return system_settings_form($form);
}
