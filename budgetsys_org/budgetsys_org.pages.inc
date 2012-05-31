<?php

function budgetsys_org_add() {
  global $user;

  
  $budget_org = entity_get_controller('budgetsys_org')->create();
  drupal_set_title(t('Create Organization'));
  return drupal_get_form('budgetsys_org_add_form', $budget_org);
  
}

/**
 * Implements hook_form().
 */
function budgetsys_org_add_form($form, &$form_state, $budget_org) {
  //Set the id to identify this as an budget system organization edit form.
  $form['#id'] = 'budgetsys-org-form';
  //Save the organization for later, in case we need it.
  $form['#budget_org'] = $budget_org;
  $form_state['budget_org'] = $budget_org;
  // Common fields. We don't have many.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => ( isset($budget_org->title) ? $budget_org->title : ''),
    '#weight' => -5,
    '#required' => TRUE,
  );
  $form['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Active Organization'),
    '#default_value' => ( isset($budget_org->active) ? $budget_org->active : 0),
    '#description' => t('Check this box is this organization is active.'),
    '#weight' => -4,
    '#required' => TRUE,
  );
    $form['budget_category'] = array(
    '#type' => 'select',
    '#description' => 'Please select the budget category this organization belongs to.',
    '#title' => t('Budget Category'),
    '#options' => array(
      'miscellaneous'=> t('Miscellaneous'),
      'media_and_publications' => t('Media & Publications'),
    ),
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