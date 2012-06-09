<?php

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


