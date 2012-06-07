<?php
/**
 * @file
 * An API for various functions of the Budget System Line module
 */

 function budgetsys_line_load_bv($account, $year) {

  $query = db_select('budgetsys_budget_value', 'bv');                     // Select FROM budget_sys
  $query->fields('bv',array('bvid','title','value'));                     // oid, o.title
  $query->condition('account_number', $account, '=');                          // WHERE active = 1
  $query->condition('year', $year, '=');  // ,budget_category = $budget_category

  $result = $query->execute()->fetchAssoc();            // Execute and place in an array keyed by the oid

  return $result;
 }

/**
 * Loads the LIDs of all the budget lines in the database
 * 
 * @return
 *  An array of all the LIDs in the system
 */
function budgetsys_line_load_all_lids() {
  
  $query = db_select('budgetsys_line', 'bl');                     // Select FROM budget_sys
  $query->fields('bl',array('lid'));                     // oid, o.title

  $result = $query->execute()->fetchCol();            // Execute and place in an array keyed by the oid
  return $result;
}