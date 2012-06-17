<?php
/**
 * @file
 * The main file for display contents on the page.
 */
/**
 * Build content for the full budget view
 */
function budgetsys_view_budget($budget_category = NULL) {
  $budget = new stdClass;
  if(!module_exists('budgetsys_org')) {
    drupal_set_message(t('The Budget System Organization module is not enabled!'), 'warning');
  }

  if(!module_exists('budgetsys_line')) {
    return drupal_set_message(t('The Budget System Line module is not enabled!<br>You cannot use this page without having this module enabled!'), 'error');
  }
  module_load_include('inc', 'budgetsys_line', 'budgetsys_line.api'); 
  $budget_line_ids = budgetsys_line_load_all_lids();
  $budget_lines = budgetsys_line_load_multiple($budget_line_ids);
  $years = budgetsys_generate_prior_years(NULL, NULL, TRUE);           // Provides the prior years for display
  $types = budgetsys_value_types_extra();                              // Gets the value types to be displayed except for the main type
  $budget->content['headers'] = array_merge($years, $types);       // Combines the arrays into 1
  foreach($budget_lines as $budget_line) {
    $line_item = budgetsys_load_complete_budget_line_items($budget_line);
    $budget->content['budget_lines'][] = $line_item;
  }
  $budget->content += array(
      '#theme' => 'budgetsys_full_budget',
      '#element' => $budget,
      '#view_mode' => 'full',
      '#language' => NULL,
  );
  drupal_set_title(variable_get('budgetsys_budget_name', '') . ' Budget');
  return $budget->content;
}


