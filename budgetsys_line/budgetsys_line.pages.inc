<?php

/**
 * Task view callback.
 */
function budgetsys_line_view($line_item) {
  drupal_set_title(entity_label('budgetsys_line', $line_item));
  return entity_view('budgetsys_line', array(entity_id('budgetsys_line', $line_item) => $line_item), 'full');
}