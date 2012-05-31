<?php

/**
 * Form builder; Build the Budget Organization settings form.
 */
function budgetsys_org_configuration_form($form, &$form_state) {
  // Selectable currency symbol used when displaying monetary values
  $form['budgetsys_org_budget_categories'] = array(
    '#type' => 'textarea',
    '#default_value' => variable_get('budgetsys_org_budget_categories',''),
    '#title' => t('Budget Categories'),
    '#required' => TRUE,
    '#description' => t('Please enter the budget categories, separated by comma followed by a space (, ).'),
  );
  
  return system_settings_form($form);
}
