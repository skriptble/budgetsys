<?php

function budgetsys_org_add() {
  global $user;

  
  $budget_org = entity_get_controller('budgetsys_org')->create();
  drupal_set_title(t('Create Organization'));
  return drupal_get_form('budgetsys_org_form', $budget_org);
  
}
/**
 * Budget System Organization edit form
 */
function budgetsys_org_page_edit ($budget_org) {
    drupal_set_title(t('<em>Edit Organization: @title</em>', array('@title' => $budget_org->title)), PASS_THROUGH);
    return drupal_get_form('budgetsys_org_form', $budget_org);
}
/**
 * Implements hook_form().
 */
function budgetsys_org_form($form, &$form_state, $budget_org) {
  //Set the id to identify this as an budget system organization edit form.
  $form['#id'] = 'budgetsys-org-form';
  //Save the organization for later, in case we need it.
  $form['#budget_org'] = $budget_org;
  $form_state['budget_org'] = $budget_org;
  // Common fields. We don't have many.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => isset($budget_org->title) ? $budget_org->title : '',
    '#weight' => -5,
    '#required' => TRUE,
  );
  $form['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Active Organization'),
    '#default_value' => isset($budget_org->active) ? $budget_org->active : 0,
    '#description' => t('Check this box is this organization is active.'),
    '#weight' => -4,
    '#required' => FALSE,
  );
    $form['budget_category'] = array(
    '#type' => 'select',
    '#description' => 'Please select the budget category this organization belongs to.',
    '#title' => t('Budget Category'),
    '#options' => budgetsys_org_list_budget_categories(),
    '#default_value' => isset($budget_org->budget_category) ? $budget_org->budget_category : 0,
    '#weight' => -3,
    '#required' => TRUE,
  );
  $form['revision'] = array(
    '#access' => user_access('administer budget values'),
    '#type' => 'checkbox',
    '#title' => t('Create new revision'),
    '#default_value' => 0,
  );
  //Add the buttons.
  $form['buttons'] = array();
  $form['buttons']['#weight'] = 100;
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
    '#submit' => array('budgetsys_org_form_submit'),
  );
  if (!empty($budget_org->oid)) {
    $form['buttons']['delete'] = array(
      '#access' => user_access('delete budget values'),
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 15,
      '#submit' => array('budgetsys_org_form_delete_submit'),
    );
  }
  $form['#validate'][] = 'budgetsys_org_form_validate';
  field_attach_form('budgetsys_org', $budget_org, $form, $form_state);

  return $form;
}
/**
* Implements hook_form_validate().
*/
function budgetsys_org_form_validate($form, &$form_state) {
  $budget_org = $form_state['budget_org'];
  // Field validation.
  field_attach_form_validate('budget_org', $budget_org, $form, $form_state);
}
/**
* Implements hook_form_submit().
*/
function budgetsys_org_form_submit($form, &$form_state) {
  global $user;
  $budget_org = &$form_state['budget_org'];
  // Set the organization's uid if it's being created at this time.
  if (empty($budget_org->uid)) {
    $budget_org->uid = $user->uid;
  }
  $budget_org->title = $form_state['values']['title'];
  $budget_org->revision = $form_state['values']['revision'];
  $budget_org->active = $form_state['values']['active'];
  $budget_org->budget_category = $form_state['values']['budget_category'];
  // Notify field widgets.
  field_attach_submit('budget_org', $budget_org, $form, $form_state);
  // Save the organization.
  budgetsys_org_save($budget_org);
  // Notify the user.
  drupal_set_message(t('Organization saved.'));
  $form_state['redirect'] = 'budget/org/' . $budget_org->oid;
}
function budgetsys_org_form_delete_submit ($form, &$form_state) {
    $destination = array();
    if (isset($_GET['destination'])) {
        $destination = drupal_get_destination();
        unset($_GET['destination']);
    }
    $budget_org = $form['#budget_org'];
    $form_state['redirect'] = array('budget/org/' . $budget_org->oid . '/delete', array('query' => $destination));
}
function budgetsys_org_delete_confirm($form, &$form_state, $budget_org) {
    $form['#budget_org'] = $budget_org;
    // Always provide entity id in the same form key as in the entity edit form.
    $form['oid'] = array('#type' => 'value', '#value' => $budget_org->oid);
    return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $budget_org->title)),
    'budget/org/' . $budget_org->oid,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
    );
}
function budgetsys_org_delete_confirm_submit($form, &$form_state) {
    if ($form_state['values']['confirm']) {
        $budget_org = budgetsys_org_load($form_state['values']['oid']);
        budgetsys_org_delete($form_state['values']['oid']);
        watchdog('budgetsys_org', 'Organization: deleted %title.', array('%title' => $budget_org->title));
        drupal_set_message(t('Organization %title has been deleted.', array('%title' => $budget_org->title)));
    }
    $form_state['redirect'] = '<front>';
}
function budgetsys_org_page_view($budget_org, $view_mode = 'full') {
    // Remove previously built content, if exists.
    $budget_org->content = array();
    if ($view_mode == 'teaser') {
      $budget_org->content['title'] = array(
        '#markup' => filter_xss($budget_org->title),
        '#weight' => -5,
      );
    }
    $budget_org->content['active'] = budgetsys_org_active_translate($budget_org->active);
    $budget_org->content['budget_category'] = $budget_org->budget_category;
    // Build fields content.
    field_attach_prepare_view('budgetsys_org', array($budget_org->oid => $budget_org), $view_mode);
    entity_prepare_view('budgetsys_org', array($budget_org->oid => $budget_org));
    $budget_org->content += field_attach_view('budgetsys_org', $budget_org, $view_mode);
    
    $budget_org->content += array(
      '#theme' => 'budgetsys_org',
      '#element' => $budget_org,
      '#view_mode' => 'full',
      '#language' => NULL,
    );
    
    return $budget_org->content;
}

function budgetsys_org_active_translate($status) {
  if($status == 1) { return 'Yes';}
  else return 'No';
}

function budgetsys_org_load_budget_categories() {
  $budget_category_s = variable_get('budgetsys_org_budget_categories');
  $budget_categories = explode(', ', $budget_category_s);
  return $budget_categories;
}

function budgetsys_org_list_budget_categories() {
  $budget_categories = budgetsys_org_load_budget_categories();
  $list = array();

  foreach($budget_categories as $category) {
    $list[t($category)] = t($category);
  }
  
  return $list;
}
