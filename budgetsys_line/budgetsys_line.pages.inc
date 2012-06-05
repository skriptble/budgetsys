<?php

/**
 * Task view callback.
 */
function budgetsys_line_view($line_item) {
  drupal_set_title(entity_label('budgetsys_line', $line_item));
  return entity_view('budgetsys_line', array(entity_id('budgetsys_line', $line_item) => $line_item), 'full');
}

function budgetsys_line_page_view($line_item, $view_mode = 'full') {
    // Remove previously built content, if exists.
    $line_item->content = array();
    if ($view_mode == 'teaser') {
      $line_item->content['title'] = array(
        '#markup' => filter_xss($line_item->title),
        '#weight' => -5,
      );
    }
    
    $line_item->content['title'] = $line_item->title;
    $line_item->content['account_number'] = $line_item->account_number;
if(module_exists('budgetsys_org')) {
    $organization = budgetsys_org_load($line_item->oid);
    $line_item->content['organization'] = $organization->title;
}  
    $line_item->content['year'] = $line_item->year;
    $line_item->content['line_item_type'] = $line_item->type;
    // Build fields content.
    field_attach_prepare_view('budgetsys_line', array($line_item->lid => $line_item), $view_mode);
    entity_prepare_view('budgetsys_line', array($line_item->lid => $line_item));
    $line_item->content += field_attach_view('budgetsys_line', $line_item, $view_mode);
    
    $line_item->content += array(
      '#theme' => 'budgetsys_line',
      '#element' => $line_item,
      '#view_mode' => 'full',
      '#language' => NULL,
    );
    
    return $line_item->content;
}