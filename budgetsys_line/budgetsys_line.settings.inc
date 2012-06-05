<?php

/**
 * Form builder; Build the Budget Line Items settings form.
 */
function budgetsys_line_configuration_form($form, &$form_state) {
  // Setting for allowing a span of 2 years for a fiscal year
  $form['budgetsys_line_multiyear_fiscal_year'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('budgetsys_line_multiyear_fiscal_year', 0),
    '#title' => t('Multiyear Fiscal Year?'),
    '#description' => t('Check this box if your organization uses a multiyear value for fiscal year. i.e. 2012-2013'),
  );
  // The current budgeting year
  $form['budgetsys_line_current_fiscal_year'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('budgetsys_line_current_fiscal_year', ''),
    '#title' => t('Current Budgeting Year'),
    '#description' => t('Please enter the year for which you are assembling a budget for. <br>If you are assembling a budget enter the current fiscal year.'),
    '#size' => 9,
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
