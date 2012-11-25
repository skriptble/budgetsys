<?php
/**
 * @file
 * The Budget System Organization API class.
 */

 class BudgetSystemOrgAPI extends BudgetSystemAPI {
   /**
   * @return $budget_categories
   * An array of budget categories from the budgetsys_org_budget_categories
   * variable.
   * 
   * Creates an array of budget_categories from the variable.
   */
  public function loadBudgetCategories() {
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
  function listBudgetCategories() {
    $budget_categories = budgetsys_org_load_budget_categories();
    $list = array();    
    foreach($budget_categories as $category) {
      $list[t(htmlspecialchars_decode($category))] = t(htmlspecialchars_decode($category));
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
  function loadBudgetOrgs($budget_category = NULL) {
  
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
  function loadOrganizationsByCategory() {
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
  function organizationSelectList() {
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
  function loadOrgBudgetLines($oid) {
  
    $query = db_select('budgetsys_line', 'l');                     // Select FROM budget_sys
    $query->fields('l',array('oid','lid', 'account_number', 'type', 'title'));                     // oid, o.title
    $query->condition('oid', $oid, '=');                          // WHERE active = 1
  
    $result = $query->execute()->fetchAllAssoc('account_number');            // Execute and place in an array keyed by the oid
    return $result;
  }
  /**
   * Loads a list of organization ids.
   * 
   * @param $budget_category
   *  An optional category that will limit the results to organizations
   * only of that category.
   * 
   * @return
   *  An array of the organization ids (oids).
   */
  function loadOids($budget_category = NULL) {
  
    $query = db_select('budgetsys_org', 'o');                     // Select FROM budget_sys
    $query->fields('o',array('oid'));                     // oid, o.title
    $query->condition('active', 1, '=');                          // WHERE active = 1
    if($budget_category) {
    $query->condition('budget_category', $budget_category, '=');  // ,budget_category = $budget_category
    }
    $result = $query->execute()->fetchCol();            // Execute and place in an array keyed by the oid
  
    return $result;
  }
 public function loadOrgAccounts($oid) {
      
    $query = db_select('budgetsys_line', 'l');                     // Select FROM budget_sys
    $query->fields('l',array('account_number'));                     // oid, o.title
    $query->condition('oid', $oid, '=');                          // WHERE active = 1
  
    $result = $query->execute()->fetchCol();            // Execute and place in an array keyed by the oid
    return $result;
  }  
}
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
  
  public $original;
}

class BudgetsysOrgClassController extends EntityAPIController {
  
  public function create(array $values = array()) {
    global $user;
    $values += array(
      'title' => '',
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
      'active' => 1,
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
       
      field_attach_presave('budgetsys_org', $budget_org);
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
        $function('budgetsys_org', $budget_org);
        module_invoke_all('entity_' . $op, $budget_org, 'budgetsys_org');
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
  
  public function loadOrgIdByCategory($category) {
    // Select the organization id from the budget value organization table where category = $category
    $query = db_query("SELECT o.oid FROM {budgetsys_org} o WHERE budget_category = :category", array(
      ':category' => $category,
    ));
     $result = $query->fetchCol(); 
      return $result;    
  }
}

/**
 * UI controller for Budget Line Type.
 */
class BudgetsysOrgUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Organizations.';
    return $items;
  }
}
