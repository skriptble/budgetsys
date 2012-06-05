<?php
/**
 * @file
 * An API for various functions of the Budget System Line module
 */

 function budgetsys_line_load_bv($account, $year) {

  $query = db_select('budgetsys_budget_value', 'bv');                     // Select FROM budget_sys
  $query->fields('bv',array('bvid','title','value'));                     // oid, o.title
  $query->condition('account_number', $account_number, '=');                          // WHERE active = 1
  $query->condition('year', $year, '=');  // ,budget_category = $budget_category

  $result = $query->execute()->fetchAllAssoc('bvid');            // Execute and place in an array keyed by the oid

  return $result;
 }
