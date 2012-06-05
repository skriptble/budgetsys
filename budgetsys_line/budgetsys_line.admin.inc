<?php

/**
 * Generates the budget type editing form.
 */
function budgetsys_line_type_form($form, &$form_state, $line_type, $op = 'edit') {

  if ($op == 'clone') {
    $line_type->label .= ' (cloned)';
    $line_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $line_type->label,
    '#description' => t('The human-readable name of this task type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($line_type->type) ? $line_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $line_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'budgetsys_line_item_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this task type. It must only contain lowercase letters, numbers, and underscores.'),
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
    '#description' => t('Please select whether this type of line item can take positive, negative, or both types of values'),
    '#required' => TRUE,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($line_type->description) ? $line_type->description : '',
    '#description' => t('Description about the task type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save budget line type'),
    '#weight' => 40,
  );

  if (!$line_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete task type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('budgetsys_line_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing budget line type.
 */
function budgetsys_line_type_form_submit(&$form, &$form_state) {
  $line_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  budgetsys_line_type_save($line_type);

  // Redirect user back to list of task types.
  $form_state['redirect'] = 'admin/structure/budget/line';
}

function budgetsys_line_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/budget/line/' . $form_state['budgetsys_line_type']->type . '/delete';
}

/**
 * Task type delete form.
 */
function budgetsys_line_type_form_delete_confirm($form, &$form_state, $line_type) {
  $form_state['line_type'] = $line_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['line_type_id'] = array('#type' => 'value', '#value' => entity_id('budgetsys_line_type' ,$line_type));
  return confirm_form($form,
    t('Are you sure you want to delete task type %title?', array('%title' => entity_label('budgetsys_line_type', $line_type))),
    'budget/line/' . entity_id('budgetsys_line_type' ,$line_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Task type delete form submit handler.
 */
function budgetsys_line_type_form_delete_confirm_submit($form, &$form_state) {
  $line_type = $form_state['line_type'];
  budgetsys_line_type_delete($line_type);

  watchdog('budgetsys_line_type', '@type: deleted %title.', array('@type' => $line_type->type, '%title' => $line_type->label));
  drupal_set_message(t('@type %title has been deleted.', array('@type' => $line_type->type, '%title' => $line_type->label)));

  $form_state['redirect'] = 'admin/structure/budget/line';
}

/**
 * Page to select budget line Type to add new task.
 */
function budgetsys_line_admin_add_page() {
  $items = array();
  foreach (budgetsys_line_item_types() as $line_type_key => $line_type) {
    $items[] = l('Add ' . entity_label('budgetsys_line_type', $line_type) . ' Line', 'budget/line/add/' . $line_type_key);
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of budget line item to create.')));
}

/**
 * Add new budget line item page callback.
 */
function budgetsys_line_add($type) {
  $line_type = budgetsys_line_item_types($type);

  $line_item = entity_create('budgetsys_line', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('budgetsys_line_type', $line_type))));

  $output = drupal_get_form('budgetsys_line_form', $line_item);

  return $output;
}

/**
 * Budget Line Form.
 */
function budgetsys_line_form($form, &$form_state, $line_item) {
  $form_state['line_item'] = $line_item;

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Account Title'),
    '#default_value' => $line_item->title,
  );

  $form['account_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Number'),
    '#default_value' => $line_item->account_number,
    '#required' => TRUE,
    '#size' => 10,
  );
  if(module_exists('budgetsys_org')) {
    module_load_include('inc', 'budgetsys_org', 'budgetsys_org.api');
  $form['oid'] = array(
    '#type' => 'select',
    '#title' => t('Organization'),
    '#default_value' => $line_item->oid,
    '#options' => budgetsys_org_organization_select_list(),  
    '#required' => TRUE,
  );
  }
   $form['year'] = array(
    '#type' => 'textfield',
    '#description' => 'The fiscal year this account is for.',
    '#title' => t('Account Year'),
    '#default_value' => $line_item->year,
    '#required' => TRUE,
    '#size' => 11,
  );
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $line_item->uid,
  );

  field_attach_form('budgetsys_line', $line_item, $form, $form_state);

  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Budget Line Item'),
    '#submit' => $submit + array('budgetsys_line_form_submit'),
  );

  // Show Delete button if we edit task.
  $line_id = entity_id('budgetsys_line' ,$line_item);
  if (!empty($line_id) && budgetsys_line_access('edit', $line_item)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('budgetsys_line_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'budgetsys_line_form_validate';

  return $form;
}

function budgetsys_line_form_validate($form, &$form_state) {

}

/**
 * Budget Line submit handler.
 */
function budgetsys_line_form_submit($form, &$form_state) {
  $line_item = $form_state['line_item'];

  entity_form_submit_build_entity('budgetsys_line', $line_item, $form, $form_state);

  budgetsys_line_save($line_item);

  $line_item_uri = entity_uri('budgetsys_line', $line_item);

  $form_state['redirect'] = $line_item_uri['path'];

  drupal_set_message(t('Task %title saved.', array('%title' => entity_label('budgetsys_line', $line_item))));
}

function budgetsys_line_form_submit_delete($form, &$form_state) {
  $line_item = $form_state['line_item'];
  $line_item_uri = entity_uri('budgetsys_line', $line_item);
  $form_state['redirect'] = $line_item_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function budgetsys_line_delete_form($form, &$form_state, $line_item) {
  $form_state['line_item'] = $line_item;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['line_type_id'] = array('#type' => 'value', '#value' => entity_id('budgetsys_line' ,$line_item));
  $line_item_uri = entity_uri('budgetsys_line', $line_item);
  return confirm_form($form,
    t('Are you sure you want to delete task %title?', array('%title' => entity_label('budgetsys_line', $line_item))),
    $line_item_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function budgetsys_line_delete_form_submit($form, &$form_state) {
  $line_item = $form_state['line_item'];
  budgetsys_line_delete($line_item);

  drupal_set_message(t('Task %title deleted.', array('%title' => entity_label('budgetsys_line', $line_item))));

  $form_state['redirect'] = '<front>';
}
