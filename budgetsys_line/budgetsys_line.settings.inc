<?php

/**
 * Form builder; Build the Budget Line Items settings form.
 */
function budgetsys_line_configuration_form($form, &$form_state) {

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
