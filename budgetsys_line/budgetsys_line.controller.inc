<?php
/**
 * @file
 * Controller for the Budget System Line Items
 */
/**
* Budget line class.
*/
class BudgetsysLineClass extends Entity {
  protected function defaultLabel() {
    return $this->title;
  }
  protected function defaultUri() {
    return array('path' => 'budget/line/' . $this->identifier());
  }
}

class BudgetsysLineClassController extends EntityAPIController {
  public function create(array $values = array()) {
    global $user;
    $values += array(
      'title' => '',
      'account_number' => '',
      'oid' => '',
      'year' => '',
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'uid' => $user->uid,
      'revision table' => 'budgetsys_line_revision',
    );
    return parent::create($values);
  }
  
  function saveRevision($budget_line, $uid, $update = FALSE) {
    // Hold on to the budget line's original creator_uid but swap
    // in the revision's creator_uid for the momentary write.
  $temp_uid = $budget_line->uid;
  $budget_line->uid = $uid;
  if ($update) {
    drupal_write_record('budgetsys_line_revision', $budget_line, 'vid');
  } 
  else {
    drupal_write_record('budgetsys_line_revision', $budget_line);
  }
  // Reset the order's creator_uid to the original value.
  $budget_line->uid = $temp_uid;
  }

}

/**
* Budget Line Type class.
*/
class LineItemType extends Entity {
  public $type;
  public $label;
  public $weight = 0;
  public function __construct($values = array()) {
    parent::__construct($values, 'budgetsys_line_type');
  }

  function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}

class LineItemTypeController extends EntityAPIControllerExportable {
   public function create(array $values = array()) {
    $values += array(
      'label' => '',
      'description' => '',
    );
    return parent::create($values);
  }

  /**
   * Save Budget Line Type.
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
 * UI controller for Budget Line Type.
 */
class LineItemTypeUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Budget Line types.';
    return $items;
  }
}