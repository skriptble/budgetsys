<?php

/**
 * Line view callback.
 */
function budgetsys_line_view($line_item) {
  drupal_set_title(entity_label('budgetsys_line', $line_item));
  return entity_view('budgetsys_line', array(entity_id('budgetsys_line', $line_item) => $line_item), 'full');
}
/**
 * Builds the variables for the page template to view a
 * budget line item's information.
 */
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
        $organization = budgetsys_org_load($line_item->oid);                // load the budget organization
        $org_path = 'budget/org/' . $organization->oid;                      
        $line_item->content['organization'] = $organization->title;         // Set the organization title
       if(user_access('view budget organizations')) {                       // If the user can view budget organization reformat the organization
           $line_item->content['organization'] = budgetsys_create_link($organization->title, $org_path); // As a link
       }
     } 
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

/**
 * Builds the content to display a full line item.
 */
function budgetsys_line_budget_view($line_item, $view_mode = 'full') {
  module_load_include('inc', 'budgetsys_line', 'budgetsys_line.api');       // Provides functions
    $line_item->content['title'] = $line_item->title;                       // Sets the title
    $line_item->content['account_number'] = $line_item->account_number;     // Sets the account number
    $line_item->content['line_item_type'] = $line_item->type;               // Sets the line item type
     if(module_exists('budgetsys_org')) {
        $organization = budgetsys_org_load($line_item->oid);                // load the budget organization
        $org_path = 'budget/org/' . $organization->oid;                      
        $line_item->content['organization'] = $organization->title;         // Set the organization title
       if(user_access('view budget organizations')) {                       // If the user can view budget organization reformat the organization
           $line_item->content['organization'] = budgetsys_create_link($organization->title, $org_path); // As a link
       }
     }       
  $account = $line_item->account_number;
  $line_item->content['values'] = budgetsys_value_load_budget_line_item($account);
  $line_item->content['num_years'] = variable_get('budgetsys_line_num_prior_years', '3');   // Provides the number of prior years to display
     $years = budgetsys_generate_prior_years(NULL, NULL, TRUE);           // Provides the prior years for display
     $types = budgetsys_value_types_extra();                              // Gets the value types to be displayed except for the main type
     $line_item->content['headers'] = array_merge($years, $types);       // Combines the arrays into 1

  
    // Build fields content.
    field_attach_prepare_view('budgetsys_line', array($line_item->lid => $line_item), $view_mode);
    entity_prepare_view('budgetsys_line', array($line_item->lid => $line_item));
    $line_item->content += field_attach_view('budgetsys_line', $line_item, $view_mode);  
      $line_item->content += array(
      '#theme' => 'budgetsys_line_budget',
      '#element' => $line_item,
      '#view_mode' => 'full',
      '#language' => NULL,
    );
    
    return $line_item->content;
}


function budgetsys_line_account_number_autocomplete_callback($string = "") {
  $matches = array();
  if ($string) {
    $result = db_select('budgetsys_line', 'bl')
      ->fields('bl', array('account_number', 'title'))
      ->condition('account_number', db_like($string) . '%', 'LIKE')
      ->range(0, 10)
      ->execute();
    foreach ($result as $line) {
      // In the simplest case (see user_autocomplete), the key and the value are the same. Here we'll display the
      // uid along with the username in the dropdown.
      $matches[$line->account_number] = check_plain($line->account_number) . " ($line->title)";
    }
  }

  drupal_json_output($matches);
}