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
      // If this is a new artwork...
      if ($budget_value->is_new) {
        // Save the new artwork.
        drupal_write_record('budgetsys_value', $budget_value);
       // Save the initial revision.
        $this->saveRevision($budget_value, $user->uid);
        $op = 'insert';
      }
        else {
          // Save the updated artwork.
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
        // If the revision ID is new or updated, save it to the artwork.
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