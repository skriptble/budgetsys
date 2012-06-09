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
      'postive' => t('Only positive values.'),
      'negative' => t('Only negative values.'),
      'both' => t('Both positive & negative values.'),
    ),
    '#description' => t('Please select whether this type of value item can take positive, negative, or both types of values'),
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



/**
 * Add new budget value item page callback.
 */
function budgetsys_value_add($type) {
  $value_type = budgetsys_value_types($type);

  $value = entity_create('budgetsys_value', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('budgetsys_value_type', $value_type))));

  $output = drupal_get_form('budgetsys_value_form', $value);

  return $output;
}

/**
 * Budget Value Form.
 */
function budgetsys_value_form($form, &$form_state, $value) {
  $form_state['value'] = $value;

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Account Title'),
    '#default_value' => $value->title,
  );

  $form['account_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Number'),
    '#default_value' => $value->account_number,
    '#required' => TRUE,
    '#size' => 10,
  );
  if(module_exists('budgetsys_org')) {
    module_load_include('inc', 'budgetsys_org', 'budgetsys_org.api');
  $form['oid'] = array(
    '#type' => 'select',
    '#title' => t('Organization'),
    '#default_value' => $value->oid,
    '#options' => budgetsys_org_organization_select_list(),  
    '#required' => TRUE,
  );
  }
   $form['year'] = array(
    '#type' => 'select',
    '#description' => 'The fiscal year this account is for.',
    '#title' => t('Account Year'),
    '#default_value' => $value->year,
    '#options' => budgetsys_load_years_list(),
    '#required' => TRUE,
  );
    $form['value'] = array(
    '#type' => 'textfield',
    '#description' => 'Only use number values with no commas or a $',
    '#title' => t('Account Value'),
    '#default_value' => $value->value,
    '#required' => TRUE,
  );
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $value->uid,
  );
  $form['revision'] = array(
    '#access' => user_access('administer budget values'),
    '#type' => 'checkbox',
    '#title' => t('Create new revision'),
    '#default_value' => 0,
  );
  field_attach_form('budgetsys_value', $value, $form, $form_state);

  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Budget Value Item'),
    '#submit' => $submit + array('budgetsys_value_form_submit'),
  );

  // Show Delete button if we edit value.
  $value_id = entity_id('budgetsys_value' ,$value);
  if (!empty($value_id) && budgetsys_value_access('edit', $value)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('budgetsys_value_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'budgetsys_value_form_validate';

  return $form;
}

function budgetsys_value_form_validate($form, &$form_state) {

}

/**
 * Budget Value submit handler.
 */
function budgetsys_value_form_submit($form, &$form_state) {
  $value = $form_state['value'];

  entity_form_submit_build_entity('budgetsys_value', $value, $form, $form_state);

  budgetsys_value_save($value);

  $value_uri = entity_uri('budgetsys_value', $value);

  $form_state['redirect'] = $value_uri['path'];

  drupal_set_message(t('Value %title saved.', array('%title' => entity_label('budgetsys_value', $value))));
}

function budgetsys_value_form_submit_delete($form, &$form_state) {
  $value = $form_state['value'];
  $value_uri = entity_uri('budgetsys_value', $value);
  $form_state['redirect'] = $value_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function budgetsys_value_delete_form($form, &$form_state, $value) {
  $form_state['value'] = $value;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['value_type_id'] = array('#type' => 'value', '#value' => entity_id('budgetsys_value' ,$value));
  $value_uri = entity_uri('budgetsys_value', $value);
  return confirm_form($form,
    t('Are you sure you want to delete value %title?', array('%title' => entity_label('budgetsys_value', $value))),
    $value_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function budgetsys_value_delete_form_submit($form, &$form_state) {
  $value = $form_state['value'];
  budgetsys_value_delete($value);

  drupal_set_message(t('Value %title deleted.', array('%title' => entity_label('budgetsys_value', $value))));

  $form_state['redirect'] = '<front>';
}