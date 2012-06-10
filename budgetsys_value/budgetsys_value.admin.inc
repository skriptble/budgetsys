<?php

/**
 * Generates the budget value type editing form.
 */
function budgetsys_value_type_form($form, &$form_state, $value_type, $op = 'edit') {

  if ($op == 'clone') {
    $value_type->label .= ' (cloned)';
    $value_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $value_type->label,
    '#description' => t('The human-readable name of this value type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($value_type->type) ? $value_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $value_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'budgetsys_value_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this value type. It must only contain lowercase letters, numbers, and underscores.'),
  );
  
  $form['value_type'] = array(
    '#title' => t('Types of Values'),
    '#type' => 'radios',
    '#default value' => '',
    '#options' => array(
      'positive' => t('Only positive values.'),
      'negative' => t('Only negative values.'),
      'both' => t('Both positive & negative values.'),
    ),
    '#description' => t('Please select whether this type of value  can take positive, negative, or both types of values'),
    '#required' => TRUE,
  );
  
  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($value_type->description) ? $value_type->description : '',
    '#description' => t('Description about the value type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save budget value type'),
    '#weight' => 40,
  );

  if (!$value_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete value type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('budgetsys_value_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing budget value type.
 */
function budgetsys_value_type_form_submit(&$form, &$form_state) {
  $value_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  budgetsys_value_type_save($value_type);

  // Redirect user back to list of value types.
  $form_state['redirect'] = 'admin/structure/budget/value';
}

function budgetsys_value_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/budget/value/' . $form_state['budgetsys_value_type']->type . '/delete';
}

/**
 * Value type delete form.
 */
function budgetsys_value_type_form_delete_confirm($form, &$form_state, $value_type) {
  $form_state['value_type'] = $value_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['value_type_id'] = array('#type' => 'value', '#value' => entity_id('budgetsys_value_type' ,$value_type));
  return confirm_form($form,
    t('Are you sure you want to delete value type %title?', array('%title' => entity_label('budgetsys_value_type', $value_type))),
    'budget/value/' . entity_id('budgetsys_value_type' ,$value_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Value type delete form submit handler.
 */
function budgetsys_value_type_form_delete_confirm_submit($form, &$form_state) {
  $value_type = $form_state['value_type'];
  budgetsys_value_type_delete($value_type);

  watchdog('budgetsys_value_type', '@type: deleted %title.', array('@type' => $value_type->type, '%title' => $value_type->label));
  drupal_set_message(t('@type %title has been deleted.', array('@type' => $value_type->type, '%title' => $value_type->label)));

  $form_state['redirect'] = 'admin/structure/budget/value';
}

/**
 * Page to select budget value Type to add new v.
 */
function budgetsys_value_admin_add_page() {
  $items = array();
  foreach (budgetsys_value_types() as $value_type_key => $value_type) {
    $items[] = l('Add ' . entity_label('budgetsys_value_type', $value_type) . ' Value', 'budget/value/add/' . $value_type_key);
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of budget value to create.')));
}



