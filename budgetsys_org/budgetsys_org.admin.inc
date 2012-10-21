<?php

/**
 * Form builder; Configure user settings for this site.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function budgetsys_org_admin_settings() {
  $form = array();
 
  return system_settings_form($form);
}