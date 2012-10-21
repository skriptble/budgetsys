<?php

/**
 * @file
 * Hooks provided by the Budget System API module
 */

 /**
  * Hooks provided by the Budget Value module
  */
 /**
 * Respond to updates to a budget value.
 *
 * This hook is invoked from budgetsys_value_save() after the budget value is updated in the budget value
 * table in the database.
 *
 * @param $budget_value
 *   The budget value that is being updated.
 *
 */
function hook_budgetsys_value_update($budget_value) {
  db_update('mytable')
    ->fields(array('extra' => $budget_value))
    ->execute();
}
