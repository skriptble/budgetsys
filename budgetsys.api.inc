<?php
/**
 * @file
 * Holds the classes for the budget system.
 */
/**
 * The main Budget System API class. Contains
 * properties for all the other API classes.
 */
class BudgetSystemAPI {
   public $settingsAPI;
   public $formattingAPI;
   
   public function __construct()
   {
     $this->settingsAPI = new BudgetSystemSettingsAPI();
     $this->formattingAPI = new BudgetSystemFormattingAPI();
   }
}
/**
 * The class for settings for The Budget System API
 */
class BudgetSystemSettingsAPI {
  
  /**
 * Creates an array of budget_categories from the variable 'budgetsys_allowed_fiscal_years'.
 * Can return either an array containing the years as values or and array keyed off of the
 * fiscal years that contains the years as values.
 * In list mode both the keys and values are run through the t() function.
 * 
 * @param $list_mode
 * If true, returns an array keyed by the fiscal year.
 * 
 * @return $fiscal_years
 * Returns an array of years or an array of years keyed by the years
 * if list_mode is true.
 */
  public function loadYears($list_mode = FALSE) {
  $fiscal_years_strings = variable_get('budgetsys_allowed_fiscal_years');
  $fiscal_years = explode(', ', $fiscal_years_strings);

  if($list_mode == TRUE) {
  $list = array();
    foreach($fiscal_years as $fiscal_year) {
      $list[t('@fiscal_year', array('@fiscal_year' => $fiscal_year))] = t('@fiscal_year', array('@fiscal_year' => $fiscal_year));
    }
    return $list;
  }
  return $fiscal_years;
  }

/**
 * Generates an array of fiscal years.
 * 
 * @param $current_year
 *  The current year, formatted as either a 2 year 9 digit definition or a 1 year 4 digit definition
 * 
 * @param $num_years
 *  The number of years to return
 * 
 * @return
 *  An array consisting of the number of years specified starting from, but not including,
 * $current_year
 */
  public function generatePriorYears($current_year = NULL, $num_years = NULL, $reversed = NULL) {
    if(!$current_year) {
      $current_year = variable_get('budgetsys_current_fiscal_year');
    }
    if(!$num_years) {
      $num_years = variable_get('budgetsys_line_num_prior_years', '3');
    }
    $multiyear_mode = variable_get('budgetsys_multiyear_fiscal_year');
    if($multiyear_mode) {
      if(strlen($current_year) != 9 || strpos($current_year, '-') != 4) { return drupal_set_message(t('The multiyear value is in an incorrect format. Ensure it is 9 digits formated as follows: 20XX-20XX<br>The current value for $current_year is: @current_year', array('@current_year' => $current_year)), 'error'); }
      $first_yr = substr($current_year, 0, 4);
      $second_yr = substr($current_year, 5, 4);
      $years = array();
      for($y=0;$y<=$num_years;$y++) {
        $temp_first_yr = $first_yr - $y;
        $temp_second_yr = $second_yr - $y;
        $years[$y] = $temp_first_yr . '-' . $temp_second_yr;
      }
      if($reversed) {
        return array_reverse($years);
      }
      return $years;
      
    }
    return drupal_set_message(t('You currently do not have MultiYear Fiscal Year mode enabled. The single year functionality has not been built yet.'), 'error');
  }

  public function getYears() {
    $num_prior_years = variable_get('budgetsys_line_num_prior_years', '');
    $current_year = variable_get('budgetsys_current_fiscal_year_taxonomy', '');
    $years = array();
    $years['current year'] = taxonomy_term_load($current_year);
    $years['prior years'] = array();
    for($i=$num_prior_years;$i>0;$i--) {
      $years['prior years'][] = variable_get('budgetsys_prior_year_' . $i);
    }
    return($years);
  }
  
  public function fiscalYearTaxonomyTree() {
    $vocabulary = variable_get('budgetsys_fiscal_years_vid', '');
    $vid = $vocabulary->vid;
    return taxonomy_get_tree($vid, 0, NULL, TRUE);
  }
  
  public function currentFiscalYear() {
    $terms = budgetsys_fiscal_year_taxonomy_tree();
    $tids = array_map('budgetsys_taxonomy_get_tid', $terms);
    $names = array_map('budgetsys_taxonomy_get_name', $terms);
    $options = array_combine($tids, $names);
    return $options;
  }

  public function taxonomyGetTID($term) {
    return $term->tid;
  }
 
  public function taxonomyGetName($term) {
    return $term->name;
  }
}

class BudgetSystemFormattingAPI {
    /**
   * A simple function to properly format currency.
   * 
   * @param $value
   *  The value to be formatted as currency.
   * 
   * @return
   *  The formatted currency.
   */  
   public function formatCurrency($value) {
    return variable_get('budgetsys_budget_value_currency_symbol', '$') . number_format($value, 2, '.', ',');
    }
  /**
   * function to create a link render item. Returns the render array.
   * 
   * @param $text
   *  The text of the link
   * 
   * @param $path
   *  The path the link leads to
   * 
   * @return
   *  see above
   */
  public function createLink($text, $path) {
    $link = array(
      '#theme' => 'link',
      '#text' => $text,
      '#path' => $path,
      '#options' => array(
        'attributes' => array(),
        'html' => FALSE,
       ),
    );  
    return $link;
  }
}
