<?php
/**
 * Add new budget value page callback.
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
    '#value' => t('Save Budget Value'),
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

/**
 * Value view callback.
 */
function budgetsys_value_view($value) {
  drupal_set_title(entity_label('budgetsys_value', $value));
  return entity_view('budgetsys_value', array(entity_id('budgetsys_value', $value) => $value), 'full');
}
/**
 * Builds the content to display a full value item.
 */
function budgetsys_value_page_view($value, $view_mode = 'full') {
    // Remove previously built content, if exists.
    $value->content = array();
    if ($view_mode == 'teaser') {
      $value->content['title'] = array(
        '#markup' => filter_xss($value->title),
        '#weight' => -5,
      );
    }
    $value->content['account_number'] = $value->account_number;
    $value->content['value'] = budget_value_format_currency($value->value);
    $value->content['year'] = $value->year;
    $value->content['type'] = $value->type;
    if(module_exists('budgetsys_org')) {
      $organization = budgetsys_org_load($value->oid);
      $value->content['organization'] = $organization->title;
    }
    // Build fields content.
    field_attach_prepare_view('budget_value', array($value->bvid => $value), $view_mode);
    entity_prepare_view('budget_value', array($value->bvid => $value));
    $value->content += field_attach_view('budget_value', $value, $view_mode);
    
    $value->content += array(
      '#theme' => 'budget_value',
      '#element' => $value,
      '#view_mode' => 'full',
      '#language' => NULL,
    );
    
    return $value->content;
}


