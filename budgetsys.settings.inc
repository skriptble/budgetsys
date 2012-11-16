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
  // The current budgeting year
  $form['budgetsys_current_fiscal_year_taxonomy'] = array(
    '#type' => 'select',
    '#default_value' => variable_get('budgetsys_current_fiscal_year_taxonomy', ''),
    '#title' => t('Current Budgeting Year'),
    '#description' => t('Please enter the year for which you are assembling a budget for. <br>If you are assembling a budget enter the current fiscal year.'),
    '#options' => budgetsys_current_fiscal_year_taxonomy(),
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
 
  $form['random_markup'] = array(
    '#markup' => budgetsys_prior_years_table(),
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
 
  // Selectable currency symbol used when displaying monetary values
  $form['budgetsys_org_budget_categories'] = array(
    '#type' => 'textarea',
    '#default_value' => variable_get('budgetsys_org_budget_categories',''),
    '#title' => t('Budget Categories'),
    '#required' => TRUE,
    '#description' => t('Please enter the budget categories, separated by comma followed by a space (, ).'),
  );
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

/**
 * Creates a table to display the prior years. These years will
 * be used throughout the system to display previous year's budget
 * values. This also clears the previous prior year variables before it
 * generates the new set.
 * 
 * returns
 * A table with headers of the prior year and number and rows of the
 * terms name.
 */
function budgetsys_prior_years_table() {
  $num_prior_years = variable_get('budgetsys_line_num_prior_years', '3');
  $terms = budgetsys_fiscal_year_taxonomy_tree();
  $tids = array_map('budgetsys_taxonomy_get_tid', $terms);
  $current_year = variable_get('budgetsys_current_fiscal_year_taxonomy', '');
  $key = array_search($current_year, $tids);
  $term = taxonomy_term_load($current_year);

  $headers = array();
  $rows = array();
  budgetsys_cleanse_prioryear_variables();
  for($i=$num_prior_years;$i>0;$i--) {
    $headers['prior_year_' . $i] = t('Prior Year @i', array('@i' => $i));
    if($key-$i >= 0) {
      $prior_term = taxonomy_term_load($tids[$key-$i]);
      $rows['prior_year_' . $i] = t($prior_term->name);
      variable_set('budgetsys_prior_year_' . $i, $prior_term);
    } else {
       $rows['prior_year_' . $i] = t('No Prior Year Exists');
    }
  }
  $headers['current_year'] =  t('Current Year');
  $rows['current_year'] = t('@term', array('@term' => $term->name));
  return theme('table', array(
    'header' => $headers,
    'rows' => array($rows),
  ));
}

/**
 * Unsets all of the previous variables so that new ones can be set.
 */
function budgetsys_cleanse_prioryear_variables() {
  $num_vars = 4;
  for($i=0;$i<$num_vars;$i++) {
    $variable_name = 'budgetsys_prior_year_' . $i;
    variable_del($variable_name);
  }
}
