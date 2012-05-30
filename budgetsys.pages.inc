<?php

function budgetsys_budget_value_add_page() {
  $item = menu_get_item();
  $links = system_admin_menu_block($item);
  foreach ($links as $link) {
    $items[] = l($link['title'], $link['href'], $item['localized_options']) . ': ' . filter_xss_admin($link['description']);
  }
  return theme('item_list', array('items' => $items));
}

function budgetsys_budget_value_add($type) {
  global $user;
  $types = budgetsys_budget_value_types();
  $type = isset($type) ? str_replace('-', '_', $type) : NULL;
  if (empty($types[$type])) {
    return MENU_NOT_FOUND;
  }
  
  $artwork = entity_get_controller('budget_value')->create($type);
  drupal_set_title(t('Create @name', array('@name' => $types[$type]->name)), PASS_THROUGH);
  return drupal_get_form($type . '_budget_value_form', $artwork);
}
/**
* Implements hook_form().
*/
function budgetsys_form($form, &$form_state, $budget_value) {
  //Set the id to identify this as an artwork edit form.
  $form['#id'] = 'budget-value-form';
  //Save the artwork for later, in case we need it.
  $form['#budget_value'] = $budget_value;
  $form_state['budget_value'] = $budget_value;
  // Common fields. We don't have many.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $budget_value->title,
    '#weight' => -5,
    '#required' => TRUE,
  );
  $form['account_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Number'),
    '#default_value' => 'N0000',
    '#weight' => -4,
    '#required' => TRUE,
  );
    $form['value'] = array(
    '#type' => 'textfield',
    '#description' => 'Only use number values with no commas or a $',
    '#title' => t('Account Value'),
    '#default_value' => '1000.00',
    '#weight' => -3,
    '#required' => TRUE,
  );
   $form['year'] = array(
    '#type' => 'textfield',
    '#description' => 'The fiscal year this account is for.',
    '#title' => t('Account Year'),
    '#default_value' => '2011-2012',
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
    '#submit' => array('budgetsys_form_submit'),
  );
  if (!empty($artwork->aid)) {
    $form['buttons']['delete'] = array(
      '#access' => user_access('delete artworks'),
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 15,
      '#submit' => array('budget_value_form_delete_submit'),
    );
  }
  $form['#validate'][] = 'budgetsys_form_validate';
  field_attach_form('budget_value', $budget_value, $form, $form_state);

  return $form;
}
/**
* Implements hook_form_validate().
*/
function budgetsys_form_validate($form, &$form_state) {
  $budget_value = $form_state['budget_value'];
  // Field validation.
  field_attach_form_validate('budget_value', $budget_value, $form, $form_state);
}
/**
* Implements hook_form_submit().
*/
function budgetsys_form_submit($form, &$form_state) {
  global $user;
  $budget_value = &$form_state['budget_value'];
  // Set the artwork's uid if it's being created at this time.
  if (empty($artwork->uid)) {
    $budget_value->uid = $user->uid;
  }
  $budget_value->title = $form_state['values']['title'];
  $budget_value->revision = $form_state['values']['revision'];
  $budget_value->account_number = $form_state['values']['account_number'];
  $budget_value->value = $form_state['values']['value'];
  $budget_value->year = $form_state['values']['year'];
  // Notify field widgets.
  field_attach_submit('budget_value', $budget_value, $form, $form_state);
  // Save the artwork.
  budget_value_save($budget_value);
  // Notify the user.
  drupal_set_message(t('Budget Value saved.'));
  $form_state['redirect'] = 'budget/value/' . $budget_value->bvid;
}
function budget_value_page_view($budget_value, $view_mode = 'full') {
    // Remove previously built content, if exists.
    $budget_value->content = array();
    if ($view_mode == 'teaser') {
      $budget_value->content['title'] = array(
        '#markup' => filter_xss($budget_value->title),
        '#weight' => -5,
      );
    }
    // Build fields content.
    field_attach_prepare_view('budget_value', array($budget_value-bvid => $budget_value), $view_mode);
    entity_prepare_view('budget_value', array($budget_value->aid => $budget_value));
    $budget_value->content += field_attach_view('budget_value', $budget_value, $view_mode);
    return $budget_value->content;
}
function budget_value_form_delete_submit ($form, &$form_state) {
    $destination = array();
    if (isset($_GET['destination'])) {
        $destination = drupal_get_destination();
        unset($_GET['destination']);
    }
    $budget_value = $form['#budget_value'];
    $form_state['redirect'] = array('budget/value/' . $budget_value->bvid . '/delete', array('query' => $destination));
}