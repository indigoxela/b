<?php
/**
 * @file
 * Render functions.
 */

/**
 * Print text to output.
 *
 * @param array $variables
 *   - value - text to print
 *   - #color - text color
 * @param bool $newline
 *   Print new line if TRUE.
 */
function render_text($variables, $newline = TRUE) {
  if (is_array($variables) && isset($variables['value'])) {
    $text = $variables['value'];
  }
  else {
    $text = $variables;
  }
  $foreground = array(
    'black' => '0;30',
    'dark_gray' => '1;30',
    'red' => '0;31',
    'bold_red' => '1;31',
    'green' => '0;32',
    'bold_green' => '1;32',
    'brown' => '0;33',
    'yellow' => '1;33',
    'blue' => '0;34',
    'bold_blue' => '1;34',
    'purple' => '0;35',
    'bold_purple' => '1;35',
    'cyan' => '0;36',
    'bold_cyan' => '1;36',
    'white' => '1;37',
    'bold_gray' => '0;37',
  );
  if (isset($variables['#color']) && isset($foreground[$variables['#color']])) {
    echo "\033[" . $foreground[$variables['#color']] . "m" . $text . "\033[0m";
  }
  else {
    echo $text;
  }
  if ($newline) {
    echo "\n";
  }
}

/**
 * If #color provided, return colored text.
 */
function table_color_text($variables) {
  $text = $variables['data'];
  $foreground = array(
    'bold' => '1',
    'black' => '0;30',
    'dark_gray' => '1;30',
    'red' => '0;31',
    'bold_red' => '1;31',
    'green' => '0;32',
    'bold_green' => '1;32',
    'brown' => '0;33',
    'yellow' => '1;33',
    'blue' => '0;34',
    'bold_blue' => '1;34',
    'purple' => '0;35',
    'bold_purple' => '1;35',
    'cyan' => '0;36',
    'bold_cyan' => '1;36',
    'white' => '1;37',
    'bold_gray' => '0;37',
  );
  if (isset($variables['#color']) && isset($foreground[$variables['#color']])) {
    return "\033[" . $foreground[$variables['#color']] . "m" . $text . "\033[0m";
  }
  else {
    return $text;
  }
}

/**
 * Main render function.
 */
function b_render($elements) {
  if (empty($elements)) {
    return;
  }

  if (!is_array($elements)) {
    echo $elements . "\n";
    return;
  }

  if (!isset($elements['#type'])) {
    foreach ($elements as $element) {
      b_render($element);
    }
  }
  else {
    $element = $elements;
    $type = $element['#type'];
    $function = 'render_' . $type;
    if (function_exists($function)) {
      $function($element);
    }
    if (isset($element['elements'])) {
      b_render($element['elements']);
    }
  }
}

/**
 * Count column width to fit screen width.
 */
function _render_table_count_columns_width($rows, $header, $delimiter, $delimiter_left, $delimiter_right) {
  $max_width = exec('tput cols');
  $cols_length = array();

  if (is_array($header)) {
    $index = 0;
    foreach ($header as $col) {
      if (is_array($col) && isset($col['data'])) {
        $length = mb_strlen($col['data']);
      }
      else {
        $length = mb_strlen($col);
      }
      if (!isset($cols_length[$index]) or $cols_length[$index] < $length) {
        $cols_length[$index] = $length;
      }
      $index++;
    }
  }
  foreach ($rows as $row) {
    $index = 0;
    foreach ($row as $col) {
      if (is_array($col) && isset($col['data'])) {
        $length = mb_strlen($col['data']);
      }
      else {
        $length = mb_strlen($col);
      }
      if (!isset($cols_length[$index]) or $cols_length[$index] < $length) {
        $cols_length[$index] = $length;
      }
      $index++;
    }
  }

  $max_size_index = 0;
  $max_length = 0;
  $total_table_width = 0;
  foreach ($cols_length as $index => $length) {
    if ($length > $max_length) {
      $max_size_index = $index;
      $max_length = $length;
    }
    $total_table_width += $length;
  }

  $total_table_width += mb_strlen($delimiter_left);
  $total_table_width += mb_strlen($delimiter_right);
  $total_table_width += mb_strlen($delimiter) * count($cols_length) - 1;

  if ($total_table_width > $max_width) {
    $diff = $total_table_width - $max_width;
    $cols_length[$max_size_index] = $cols_length[$max_size_index] - $diff;
  }
  return $cols_length;
}

/**
 * Return column text based on width.
 */
function _render_print_col($col, $width) {
  $color_width = 0;
  if (is_array($col) && isset($col['data'])) {
    $data = table_color_text($col);
    $color_width = mb_strlen($data) - mb_strlen($col['data']);
  }
  else {
    $data = $col;
  }
  return sprintf('%-' . ($width + $color_width) . 's', $data);
}

/**
 * Display a table of information.
 */
function render_table($variables) {
  $header = FALSE;
  if (isset($variables['header'])) {
    $header = $variables['header'];
  }
  $rows = $variables['rows'];
  $delimiter = isset($variables['delimiter']) ? $variables['delimiter'] : " | ";
  $delimiter_left = isset($variables['delimiter_left']) ? $variables['delimiter_left'] : "| ";
  $delimiter_right = isset($variables['delimiter_right']) ? $variables['delimiter_right'] : " |";
  $max_width = exec('tput cols');

  $cols_length = _render_table_count_columns_width($rows, $header, $delimiter, $delimiter_left, $delimiter_right);

  if ($header) {
    $row_rendered = array();
    foreach ($header as $index => $col) {
      $row_rendered[] = _render_print_col($col, $cols_length[$index]);
    }
    // Flip background and foreground colors for table header.
    echo "\033[7m" . $delimiter_left . implode($delimiter, $row_rendered) . $delimiter_right . "\033[0m\n";
  }

  foreach ($rows as $row) {
    $print_args = array();
    $repeat = FALSE;
    $row_rendered = array();
    foreach ($row as $index => $col) {

      $data = '';
      $col_len = 0;
      if (is_array($col) && isset($col['data'])) {
        if (mb_strlen($col['data']) > $cols_length[$index]) {
          $repeat = $col;
          $repeat['index'] = $index;
          $repeat['data'] = mb_substr($col['data'], $cols_length[$index]);
          $col['data'] = mb_substr($col['data'], 0, $cols_length[$index]);
        }
      }
      else {
        if (mb_strlen($col) > $cols_length[$index]) {
          $repeat = array(
            'index' => $index,
            'data' => mb_substr($col, $cols_length[$index]),
          );
          $col = mb_substr($col, 0, $cols_length[$index]);
        }
      }
      $row_rendered[] = _render_print_col($col, $cols_length[$index]);
    }
    echo $delimiter_left . implode($delimiter, $row_rendered) . $delimiter_right . "\n";
    if ($repeat) {
      $rows_return = _row_next_line_value($repeat, $cols_length, $delimiter);
      foreach ($rows_return as $row_return) {
        echo $delimiter_left . implode($delimiter, $row_return) . $delimiter_right . "\n";
      }
    }
  }
}

/**
 * Output next value line to fit column size.
 *
 * If column does not fit screen size and column size, we use this function to
 * properly output next value line to fit column size.
 */
function _row_next_line_value($repeat, $cols_length) {
  $row_rendered = array();
  $rows_return = array();
  foreach ($cols_length as $index => $length) {
    if ($index == $repeat['index']) {
      $col = $repeat;
      if (mb_strlen($col['data']) > $length) {
        $col['data'] = mb_substr($col['data'], 0, $length);
        $repeat['data'] = mb_substr($repeat['data'], $length);
      }
      else {
        $repeat = FALSE;
      }
      $row_rendered[$index] = _render_print_col($col, $length);
    }
    else {
      $row_rendered[$index] = _render_print_col('', $length);
    }
  }
  $rows_return[] = $row_rendered;
  if ($repeat) {
    $tmp = _row_next_line_value($repeat, $cols_length);
    foreach ($tmp as $tmp_row) {
      $rows_return[] = $tmp_row;
    }
  }

  return $rows_return;
}
