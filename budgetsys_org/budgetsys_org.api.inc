<?php
/**
 * @file
 * Various API functions for the other parts of the
 * Budget System to use to interact with
 * Budget System Organizations
 */
/**
 * @return $budget_categories
 * An array of budget categories from the budgetsys_org_budget_categories
 * variable.
 * 
 * Creates an array of budget_categories from the variable.
 */
function budgetsys_org_load_budget_categories() {
  $budget_category_s = variable_get('budgetsys_org_budget_categories');
  $budget_categories = explode(', ', $budget_category_s);
  return $budget_categories;
}
/**
 * @return $list
 * An array of budget categories, keyed by the category name
 * 
 * Generates a list of budget categories. Both the index
 * and the values are the translated budget category
 */
function budgetsys_org_list_budget_categories() {
  $budget_categories = budgetsys_org_load_budget_categories();
  $list = array();

  foreach($budget_categories as $category) {
    $list[t($category)] = t($category);
  }
  
  return $list;
}
/**
 * Loads a list of organizations into an array.
 * 
 * The each item in the array keyed by the oid.
 * 
 * @param $budget_category
 *  An optional category that will limit the results to organizations
 * only of that category.
 * 
 * @return
 *  An array of the organizations and oids keyed by the oid.
 */
function budgetsys_org_load_budget_orgs($budget_category = NULL) {

  $query = db_select('budgetsys_org', 'o');                     // Select FROM budget_sys
  $query->fields('o',array('oid','title'));                     // oid, o.title
  $query->condition('active', 1, '=');                          // WHERE active = 1
  if($budget_category) {
  $query->condition('budget_category', $budget_category, '=');  // ,budget_category = $budget_category
  }
  $result = $query->execute()->fetchAllAssoc('oid');            // Execute and place in an array keyed by the oid

  return $result;
}
/**
 * Loads a list of organizations as an array with a key of the oid 
 * and a value of the organization's title organized by the budget
 * category.
 * 
 * @return
 *  An multidimensional array. The first dimension contains the budget
 * categories. The second dimension contains the oids of the organization.
 * The value of each item is the title of the organization
 */
function budgetsys_org_load_organizations_by_category() {
  $organization_list = array();                                       // Sets up the array to be returned
  $budget_categories = budgetsys_org_load_budget_categories();        // Loads a list of the Budget Categories
  foreach($budget_categories as $budget_category) {
    $organizations = budgetsys_org_load_budget_orgs($budget_category);// Retrieves a list of organizations with that budget category
    $organization_list[$budget_category][-1] = $budget_category;    // Assigns the budget category to -1 so it won't be included as an oid
    foreach($organizations as $oid => $organization) {
      $organization_list[$budget_category][$oid] = $organization->title;  // Sets the array item key to the organization id and the value to the organization title
    }
  }
  return $organization_list; 
}
/**
 * Returns a multi-dimensional array. The first level is a list of budget categories
 * which. The second level are the actual selectable organizations.
 * 
 * @return
 *  An multi-dimensional array of organizations
 */
function budgetsys_org_organization_select_list() {
  $organizations = budgetsys_org_load_organizations_by_category(); // Loads the organizations into the $organizations array
 foreach ($organizations as $budget_categories) {
   $items[$budget_categories[-1]] = array();                      // Makes an array with a key of the budget category
   $budget_category = $budget_categories[-1];                     // Sets the budget category into its own variable 
   unset($budget_categories[-1]);                                 // Removes the budget category from the array so it does not get printed in the list
    foreach($budget_categories as $oid =>$organization) {
      $items[$budget_category][$oid] = $organization;                 // Fills the budget category array with the associated organizations
    }
  }
      return $items;
}

function budgetsys_org_load_org_budget_lines($oid) {

  $query = db_select('budgetsys_line', 'l');                     // Select FROM budget_sys
  $query->fields('l',array('oid','lid', 'account_number', 'type', 'title'));                     // oid, o.title
  $query->condition('oid', $oid, '=');                          // WHERE active = 1

  $result = $query->execute()->fetchAllAssoc('account_number');            // Execute and place in an array keyed by the oid
  return $result;
}
