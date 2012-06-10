<?php

function budgetsys_budget_value_overview_types() {
  foreach (budgetsys_budget_value_types() as $type => $info) {
    $type_url_str = str_replace('_', '-', $type);
    $label = t('View @type', array('@type' => $info->name));
    $items[] = l($label, 'admin/structure/budgetvalue/manage/' . $type_url_str);
  }
  return theme('item_list', array('items' => $items));
}

function budgetsys_budget_value_information($budget_value_type) {
  return $budget_value_type->name . ': ' . $budget_value_type->description;
}