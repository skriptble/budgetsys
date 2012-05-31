<?php

/**
 * @file
 * The controller for Budget System Organization
 */
/**
 * Budget System Organization class.
 */
class BudgetsysOrgClass extends Entity {
  protected function defaultLabel() {
    return $this->title;
  }
  
  protected function defaultUri() {
    return array('path' => 'budget/org/' . $this->identifier());
  }
}

class BudgetsysOrgClassController extends EntityAPIController {
  
  public function create(array $values = array()) {
    global $user;
    $values += array(
      'title' => '',
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'active' => 0,
      'budget category' => '',
      'uid' => $user->uid,
    );
    return parent::create($values);
  }
  public function save($budget_org) {
    $transaction = db_transaction();
    try {
      global $user;
      // Determine if we will be inserting a new artwork.
      $budget_org->is_new = empty($budget_org->oid);
      // Set the timestamp fields
      if (empty($budget_org->created)) {
        $budget_org->created = REQUEST_TIME;
      }
      $budget_org->changed = REQUEST_TIME;
      $budget_org_revision_timestamp = REQUEST_TIME;
      $update_budget_org = TRUE;
      // Give modules the opportunity to prepare field data for
      // saving.
      field_attach_presave('budget_org', $budget_org);
      if (!$budget_org->is_new && !empty($budget_org->revision) && $budget_org->vid) {
        $budget_org->old_vid = $budget_org->vid;
        unset($budget_org->vid);
      }
      // If this is a new artwork...
      if ($budget_org->is_new) {
        // Save the new artwork.
        drupal_write_record('budgetsys_org', $budget_org);
       // Save the initial revision.
        $this->saveRevision($budget_org, $user->uid);
        $op = 'insert';
      }
        else {
          // Save the updated artwork.
          drupal_write_record('budgetsys_org', $budget_org, 'oid');
          if (!empty($budget_org->revision)) { 
            $this->saveRevision($budget_org, $user->uid);
          }
          else {
            $this->saveRevision($budget_org, $user->uid, TRUE);
            $update_budget_org = FALSE;
          }
          $op = 'update';
        }
        // If the revision ID is new or updated, save it to the artwork.
        if ($update_budget_org) {
          db_update('budgetsys_org')
          ->fields(array('vid' => $budget_org->vid))
          ->condition('oid', $budget_org->oid)
          ->execute();
        }
        // Save fields.
        $function = 'field_attach_' . $op;
        $function('budget_org', $budget_org);
        module_invoke_all('entity_' . $op, $budget_org, 'budget_org');
        // Clear internal properties.
        unset($budget_org->is_new);
        // Ignore slave server temporarily to give time for the saved
        // order to be propogated to the slave.
        db_ignore_slave();
        return $budget_org;
      }
      catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('budgetsys_org', $e, NULL, WATCHDOG_ERROR);
      return FALSE;
    }
  } 
  function saveRevision($budget_org, $uid, $update = FALSE) {
    // Hold on to the artwork's original creator_uid but swap
    // in the revision's creator_uid for the momentary write.
  $temp_uid = $budget_org->uid;
  $budget_org->uid = $uid;
  if ($update) {
    drupal_write_record('budgetsys_org_revision', $budget_org, 'vid');
  } 
  else {
    drupal_write_record('budgetsys_org_revision', $budget_org);
  }
  // Reset the order's creator_uid to the original value.
  $budget_org->uid = $temp_uid;
  }
}
