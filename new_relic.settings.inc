<?php

/**
 * @file
 * New Relic Backdrop setting form and callbacks.
 */

/**
 * New Relic settings form.
 */
function new_relic_settings($form, &$form_state) {
  $config = config_get('new_relic.settings');
  $form = array();

  $form['new_relic_track_drush'] = array(
    '#type' => 'select',
    '#title' => t('Drush transactions'),
    '#description' => t('How do you wish to track drush commands?'),
    '#options' => array(
      'ignore' => t('Ignore completely'),
      'bg' => t('Track as background tasks'),
      'norm' => t('Track normally'),
    ),
    '#default_value' => $config->get('track_drush', 'norm'),
  );

  $form['new_relic_track_cron'] = array(
    '#type' => 'select',
    '#title' => t('Cron transactions'),
    '#description' => t('How do you wish to track cron tasks?'),
    '#options' => array(
      'ignore' => t('Ignore completely'),
      'bg' => t('Track as background tasks'),
      'norm' => t('Track normally'),
    ),
    '#default_value' => $config->get('track_cron', 'norm'),
  );

  $form['new_relic_module_deployment'] = array(
    '#type' => 'select',
    '#title' => t('Track module activation as deployment'),
    '#description' => t('Turning this on will create a "deployment" on the New Relic dashboard each time a module is enabled or disabled. This will help you track before and after statistics.'),
    '#options' => array(
      '1' => t('Enable'),
      '0' => t('Disable'),
    ),
    '#default_value' => $config->get('module_deployment', 0),
  );

  $form['new_relic_add_manual_rum_instrumentation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Manually add RUM instructions to header and footer'),
    '#description' => t("When compress cached pages and cache pages for anonymous users is turned on, the New Relic Browser JavaScript is not inserted into the served pages for anonymous users. This is because Backdrop's pages are compressed before they are stored in the cache (with gzip), so New Relic's PHP agent does not have a chance to parse any HTML, to add header and footer automatically. In this situation, manual instrumentation provides a better opportunity to capture data for anonymous users. See also @reference_url.", array('@reference_url' => 'https://docs.newrelic.com/docs/agents/php-agent/frameworks-libraries/drupal-specific-functionality#page-load-timing-rum')),
    '#default_value' => $config->get('add_manual_rum_instrumentation', FALSE),
  );

  $form['new_relic_ignore_urls'] = array(
    '#type' => 'textarea',
    '#wysiwyg' => FALSE,
    '#title' => t('Ignore URLs'),
    '#description' => t('Enter URLs you wish New Relic to ignore. Enter one URL per line.'),
    '#default_value' => $config->get('ignore_urls', ''),
  );

  $form['new_relic_ignored_roles'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t('Ignored Roles'),
    '#required' => FALSE,
    '#options' => user_roles(),
    '#description' => t('Select roles you wish New Relic to ignore.'),
    '#default_value' => $config->get('ignored_roles', array()),
  );

  $form['new_relic_bg_urls'] = array(
    '#type' => 'textarea',
    '#wysiwyg' => FALSE,
    '#title' => t('Background URLs'),
    '#description' => t('Enter URLs you wish to have tracked as background tasks. Enter one URL per line.'),
    '#default_value' => $config->get('bg_urls', ''),
  );

  $form['new_relic_exclusive_urls'] = array(
    '#type' => 'textarea',
    '#wysiwyg' => FALSE,
    '#title' => t('Exclusive URLs'),
    '#description' => t('Enter URLs you wish exclusively track. This is usefull for debugging specific issues. **NOTE** Entering URLs here effectively marks all other URLs as ignored. Leave blank to disable.'),
    '#default_value' => $config->get('exclusive_urls', ''),
  );

  $form['new_relic_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API Key'),
    '#description' => t('Enter your New Relic API key if you wish to view reports and analysis within Backdrop'),
    '#default_value' => $config->get('api_key', ''),
  );

  $form['new_relic_watchdog_severities'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t('Forward watchdog messages'),
    '#description' => t('Select which watchdog severities should be forwarded to New Relic API as errors.'),
    '#options' => watchdog_severity_levels(),
    '#default_value' => $config->get('watchdog_severities', array()),
  );

  $form['new_relic_override_exception_handler'] = array(
    '#type' => 'checkbox',
    '#title' => t('Override exception handler'),
    '#description' => t('Check to override default Backdrop exception handler and to have exceptions passed to New Relic'),
    '#default_value' => $config->get('override_exception_handler', FALSE),
  );

  $form['new_relic_use_menu_item_as_transaction'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use the active menu item to name transactions'),
    '#description' => t('If enabled the path of the active menu item on a request will be used as transaction name in New Relic. This allows you to use fine granular key transaction tracking.'),
    '#default_value' => $config->get('use_menu_item_as_transaction', FALSE),
  );

  $form['new_relic_add_node_type_to_node_page_paths'] = array(
    '#type' => 'checkbox',
    '#title' => t('Put the node type in the paths for node pages'),
    '#description' => t('If enabled the node type will be inserted into the path where the nid placeholder would be. This depends on using the menu item path for the name.'),
    '#default_value' => $config->get('add_node_type_to_node_page_paths', FALSE),
  );

  $form['new_relic_suppress_module_not_enabled_error_on_cli'] = array(
    '#type' => 'checkbox',
    '#title' => t('Suppress error message on CLI when New Relic extension is not present'),
    '#description' => t('By default the module will show an error message when New Relic extension is not present. Suppress that message only when backdrop invoked from command line interface, for example by drush.'),
    '#default_value' => $config->get('suppress_module_not_enabled_error_on_cli', FALSE),
  );

  $form['new_relic_suppress_module_not_enabled_error_always'] = array(
    '#type' => 'checkbox',
    '#title' => t('Always suppress error message when New Relic extension is not present'),
    '#description' => t('By default the module will show an error message when New Relic extension is not present. Suppress that message always, for example on a local development environment.'),
    '#default_value' => $config->get('suppress_module_not_enabled_error_always', FALSE),
  );

  return system_settings_form($form);
}

/**
 * Form callback for manually creating a deployment.
 */
function new_relic_deploy($form, &$form_state) {
  $form = array();

  $form['deploy_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Deployer/Deployment Name'),
    '#required' => TRUE,
    '#description' => t('Enter the name for this deployment of your application. This will be the name shown in your list of deployments on the New Relic website.'),
  );

  $form['deploy_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Deployment Description'),
    '#description' => t('Provide some notes and description regarding this deployment.'),
  );

  $form['deploy_changelog'] = array(
    '#type' => 'textarea',
    '#title' => t('Deployment Changelog'),
    '#description' => t('Provide a specific changelog for this deployment.'),
  );

  $form['deploy_revision'] = array(
    '#type' => 'textfield',
    '#title' => t('Deployment Revision'),
    '#description' => t('Add a revision number to this deployment.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create Deployment'),
  );

  return $form;
}

/**
 * FormAPI sumbit callback for the manual deployment creation.
 */
function new_relic_deploy_submit($form, &$form_state) {

  $deployments = _new_relic_deploy($form_state['values']['deploy_user'],
    $form_state['values']['deploy_description'],
    $form_state['values']['deploy_changelog'],
    $form_state['values']['deploy_revision']);

  if (strlen($deployments) > 20) {
    backdrop_set_message(t('New Relic deployment created successfully'), 'status');
  }
  else {
    backdrop_set_message(t('New Relic deployment failed to be created. Please ensure you have your account configured on the <a href="@settings">New Relic Backdrop admin page</a>.', array('@settings' => url('admin/config/development/new-relic'))), 'error');
  }
}
