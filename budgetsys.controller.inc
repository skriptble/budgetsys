<?php

class BudgetsysBudgetValueController extends DrupalDefaultEntityController {
public function create($type = '') {
    return (object) array(
      'bvid' => '',
      'type' => $type,
      'title' => '',
    );
  }
 public function save($budget_value) {
    $transaction = db_transaction();
    try {
      global $user;
      // Determine if we will be inserting a new artwork.
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
        drupal_write_record('budgetsys_budget_value', $budget_value);
       // Save the initial revision.
        $this->saveRevision($budget_value, $user->uid);
        $op = 'insert';
      }
        else {
          // Save the updated artwork.
          drupal_write_record('budgetsys_budget_value', $budget_value, 'bvid');
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
          db_update('budgetsys_budget_value')
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
    // Hold on to the artwork's original creator_uid but swap
    // in the revision's creator_uid for the momentary write.
    $temp_uid = $budget_value->uid;
  $budget_value->uid = $uid;
  if ($update) {
    drupal_write_record('budgetsys_budget_value_revision', $budget_value, 'vid');
  } 
  else {
    drupal_write_record('budgetsys_budget_value_revision', $budget_value);
  }
  // Reset the order's creator_uid to the original value.
  $budget_value->uid = $temp_uid;
  }
public function delete($bvids) {
  if (!empty($bvids)) {
      $budget_value = $this->load($bvids, array());
      $transaction = db_transaction();
      try {
          db_delete('budgetsys_budget_value')
          ->condition('bvid', $bvids, 'IN')
          ->execute();
          db_delete('budgetsys_budget_value_revision')
          ->condition('bvid', $bvids, 'IN')
          ->execute();
          foreach ($budget_values as $budget_value_id => $budget_value) {
              field_attach_delete('budget_value', $budget_value);
          }
          db_ignore_slave();
      }
      catch (Exception $e) {
          $transaction->rollback();
          watchdog_exception('budget_value', $e, NULL, WATCHDOG_ERROR);
          return FALSE;
      }
      module_invoke_all('entity_delete', $budget_value, 'budget_value');
      // Clear the page and block and artwork caches.
      cache_clear_all();
      $this->resetCache();
  }  
  return TRUE;
  }
}
