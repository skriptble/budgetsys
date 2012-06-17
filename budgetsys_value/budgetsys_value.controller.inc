<?php
/**
 * @file
 * Controller for the Budget System Values
 */
/**
* Budget value class.
*/
class BudgetsysValueClass extends Entity {
  protected function defaultLabel() {
    return $this->title;
  }
  protected function defaultUri() {
    return array('path' => 'budget/value/' . $this->identifier());
  }
}
class BudgetsysValueController extends EntityAPIController {
  public function create(array $values = array()) {
    global $user;
    $values += array(
      'bvid' => '',
      'title' => '',
      'year' => variable_get('budgetsys_current_fiscal_year'),
      'oid' => NULL,
      'account_number' => '',
      'uid' => $user->uid,
      'value' => '',
    );
    return parent::create($values);
  }
  public function save($budget_value) {
    $transaction = db_transaction();
    try {
      global $user;
      // Determine if we will be inserting a new budget value.
      $budget_value->is_new = empty($budget_value->bvid);
      // Set the timestamp fields
      if (empty($budget_value->created)) {
        $budget_value->created = REQUEST_TIME;
      }
      $budget_value->changed = REQUEST_TIME;
      $budget_value_revision_timestamp = REQUEST_TIME;
      $update_budget_value = TRUE;
      // Give modules the opportunity to prepare field data for
      // saving.
      field_attach_presave('budget_value', $budget_value);
      if (!$budget_value->is_new && !empty($budget_value->revision) && $budget_value->vid) {
        $budget_value->old_vid = $budget_value->vid;
        unset($budget_value->vid);
      }
      // If this is a new budget value...
      if ($budget_value->is_new) {
        // Save the new budget value.
        drupal_write_record('budgetsys_value', $budget_value);
       // Save the initial revision.
        $this->saveRevision($budget_value, $user->uid);
        $op = 'insert';
      }
        else {
          // Save the updated budget value.
          drupal_write_record('budgetsys_value', $budget_value, 'bvid');
          if (!empty($budget_value->revision)) { 
            $this->saveRevision($budget_value, $user->uid);
          }
          else {
            $this->saveRevision($budget_value, $user->uid, TRUE);
            $update_budget_value= FALSE;
          }
          $op = 'update';
        }
        // If the revision ID is new or updated, save it to the budget value.
        if ($update_budget_value) {
          db_update('budgetsys_value')
          ->fields(array('vid' => $budget_value->vid))
          ->condition('bvid', $budget_value->bvid)
          ->execute();
        }
        // Save fields.
        $function = 'field_attach_' . $op;
        $function('budget_value', $budget_value);
        module_invoke_all('entity_' . $op, $budget_value, 'budget_value');
        // Clear internal properties.
        unset($budget_value->is_new);
        // Ignore slave server temporarily to give time for the saved
        // order to be propogated to the slave.
        db_ignore_slave();
        return $budget_value;
      }
      catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('budgetsys', $e, NULL, WATCHDOG_ERROR);
      return FALSE;
    }
  } 
  function saveRevision($budget_value, $uid, $update = FALSE) {
        // Hold on to the budget value's original creator_uid but swap
        // in the revision's creator_uid for the momentary write.
      $temp_uid = $budget_value->uid;
      $budget_value->uid = $uid;
      if ($update) {
        drupal_write_record('budgetsys_value_revision', $budget_value, 'vid');
      } 
      else {
        drupal_write_record('budgetsys_value_revision', $budget_value);
      }
      // Reset the order's creator_uid to the original value.
      $budget_value->uid = $temp_uid;
  }
/**
 * Queries the budgetsys_value table for an budget value ID. Used to determine if a budget value already exists. To load a budget value
 * for use in a budget line use loadBudgetValue
 */
  public function queryValue($account = NULL, $type = NULL, $year = NULL) {
    // Select the budget value id from the budget value table where the account number = $account and the type = $type and the year = $year
    $query = db_query("SELECT b.bvid FROM {budgetsys_value} b WHERE account_number = :account AND year = :year AND type = :type", array(
      ':account' => $account,
      ':year' => $year,
      ':type' => $type,
    ));
     $result = $query->fetchCol(); 
      return $result;
  }
/**
 * Loads an individual line item from the budgetsys_value table.
 * 
 * @param $account
 *  The account number for the budget value
 * 
 * @param $year
 *  The fiscal year for the budget value
 * 
 * @param $type
 *  The value type for the budget value
 * 
 * @return
 *  An array containing the bvid, title, and value if there is a match
 */
  public function loadBudgetValue($account, $year, $type) {

  // Select the budget value id, title, and value from the budget value table where the account number = $account and year = $year
  $query = db_query("SELECT b.bvid, b.title, b.value FROM {budgetsys_value} b WHERE account_number = :account AND year = :year AND type = :type", array(
    ':account' => $account,
    ':year' => $year,
    ':type' => $type,
  ));
  $result = $query->fetchAssoc();
  return $result;
 }
  
/**
 * Takes an account number and optionally a set of years and returns an array.
 * This array contains the budget values, if they exist for the accounts for
 * the specified years. If the user can edit the budget values it returns them
 * as a link to the edit page.
 * 
 * @param $account
 *  The account number that needs to be displayed
 * 
 * @param $years
 *  An optional array of fiscal years
 * 
 * @return
 *  An array of budget values 
 */
function loadBudgetLineItem($account, $main_type = NULL, $types = NULL ,$years = NULL) {
  module_load_include('inc', 'budgetsys_line', 'budgetsys_line.api'); 
   if($years == NULL) {
      $years = budgetsys_generate_prior_years(NULL, NULL, TRUE);                // Gets the prior years to be displayed
    }
   if($main_type == NULL) {
     $main_type = variable_get('budgetsys_value_final_type');
     if($main_type == NULL) {
       return drupal_set_message("The Final Budget Value Type has not been set. Please set it in the configuration form for the Budget System Value module.");
     }
   }
  if($types == NULL) {
    $types_raw = budgetsys_value_types();
    $types = array();
    foreach($types_raw as $type) {
      $types[] = $type->type;
    }
     $main_type_key = array_search($main_type, $types);
     unset($types[$main_type_key]);
  }
  foreach($years as $year) {    
    $temp_line = budgetsys_value_load_budget_value($account, $year, $main_type);                   // Loads the budget value
    if($temp_line['bvid'] && user_access('edit budget values')) {           // If the budget value exists and the user can edit budget values
      $formatted_line = budget_value_format_currency($temp_line['value']);  // format this number as a link to the edit page
      $path = 'budget/value/'. $temp_line['bvid'] . '/edit';
      $line_item[] = budgetsys_create_link($formatted_line, $path); 
    }
    else {
       $line_item[] = budget_value_format_currency($temp_line['value']); // If the user cannot edit budget values or this budget value
     }                                                                             // doesn't exist format it as a regular value
  }
  foreach($types as $type) {    
    $temp_line = budgetsys_value_load_budget_value($account, $year, $type);                   // Loads the budget value
    if($temp_line['bvid'] && user_access('edit budget values')) {           // If the budget value exists and the user can edit budget values
      $formatted_line = budget_value_format_currency($temp_line['value']);  // format this number as a link to the edit page
      $path = 'budget/value/'. $temp_line['bvid'] . '/edit';
      $line_item[] = budgetsys_create_link($formatted_line, $path); 
    }
    else {
       $line_item[] = budget_value_format_currency($temp_line['value']); // If the user cannot edit budget values or this budget value
     }                                                                             // doesn't exist format it as a regular value
  }
  return $line_item;
}
  
}

/**
* Budget Value Type class.
*/
class BudgetValueType extends Entity {
  public $type;
  public $label;
  public $weight = 0;
  public function __construct($values = array()) {
    parent::__construct($values, 'budgetsys_value_type');
  }

  function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}

class BudgetValueTypeController extends EntityAPIControllerExportable {
   public function create(array $values = array()) {
    $values += array(
      'label' => '',
      'description' => '',
    );
    return parent::create($values);
  }

  /**
   * Save Budget Value Type.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    parent::save($entity, $transaction);
    // Rebuild menu registry. We do not call menu_rebuild directly, but set
    // variable that indicates rebuild in the end.
    // @see _http://drupal.org/node/1399618
    variable_set('menu_rebuild_needed', TRUE);
  }
}

/**
 * UI controller for Budget Value Type.
 */
class BudgetValueTypeUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Budget Value types.';
    return $items;
  }
}