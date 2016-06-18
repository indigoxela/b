<?php
/*
 * @file
 * Common functions.
 */

/**
 * First bootstrap phase: initialize configuration.
 */
define('B_BOOTSTRAP_CONFIGURATION', 0);

/**
 * Second bootstrap phase: try to serve a cached page.
 */
define('B_BOOTSTRAP_PAGE_CACHE', 1);

/**
 * Third bootstrap phase: initialize database layer.
 */
define('B_BOOTSTRAP_DATABASE', 2);

/**
 * Fourth bootstrap phase: initialize the lock system.
 */
define('B_BOOTSTRAP_LOCK', 3);

/**
 * Fifth bootstrap phase: initialize the variable system.
 */
define('B_BOOTSTRAP_VARIABLES', 4);

/**
 * Sixth bootstrap phase: initialize session handling.
 */
define('B_BOOTSTRAP_SESSION', 5);

/**
 * Seventh bootstrap phase: set up the page header.
 */
define('B_BOOTSTRAP_PAGE_HEADER', 6);

/**
 * Eighth bootstrap phase: find out language of the page.
 */
define('B_BOOTSTRAP_LANGUAGE', 7);

/**
 * Final bootstrap phase: Backdrop is fully loaded; validate and fix input data.
 */
define('B_BOOTSTRAP_FULL', 8);

function b_print_error($text) {
  b_render(array(
    '#type' => 'text',
    '#color' => 'red',
    'value' => $text,
  )
  );
}

function b_messages($message = FALSE, $type = FALSE) {
  static $messages = array();
  if($message) {
    if(!isset($messages[$type])){
      $messages[$type] = array();
    }
    $messages[$type][] = $message;
  }
  return $messages;
}

function b_set_message($message = FALSE, $type = 'status') {
  b_messages($message, $type);
}

function b_print_messages() {
  $messages = b_messages();

  if(empty($messages)){
    return;
  }
  $rows = array();
  foreach($messages as $type => $sub_messages) {
    if($type == 'log' && !b_is_debug()) {
      continue;
    }
    switch($type) {
      case 'status':
        $color = 'bold_blue';
        $symbol = '[o]';
        break;
      case 'success':
        $color = 'green';
        $symbol = '[v]';
        break;
      case 'warning':
        $color = 'yellow';
        $symbol = '[!]';
        break;
      case 'error':
        $color = 'red';
        $symbol = '[x]';
        break;
      case 'log':
        $color = 'red';
        $symbol = ' - ';
        break;
      default:
        $color = 'red';
        $symbol = '[x]';
        break;
    };
    foreach($sub_messages as $message) {
      $rows[] = array(
        array('data' => $symbol, '#color' => $color),
        $message,
      );
    }
  }
  $output[] = array(
    '#type' => 'table',
    'rows' => $rows,
    'delimiter' => '   ',
    'delimiter_left' => ' ',
    'delimiter_right' => ' ',
  );

  b_render($output);
  echo "\n";

}

/**
 * Set drush compatible mode and return status.
 */
function drush_mode($set = NULL) {
  static $mode;
  
  if($set != NULL) {
    $mode = $set;
  }
  return $mode;
}

/**
 * Set Yes to all mode and return status.
 */
function b_yes_mode($set = NULL) {
  static $mode;
  
  if($set != NULL) {
    $mode = $set;
  }
  return $mode;
}

/**
 * If -d specified.
 */
function b_is_debug($set = NULL) {
  static $mode;
  
  if($set != NULL) {
    $mode = $set;
  }
  return $mode;
}