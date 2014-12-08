<?php

/**
 * Implements hook_element_info
 * @return boolean
 */
function ka_upload_element_info() {
    return array(
        'ka_upload' => array(
            '#input' => TRUE,
            '#process' => array('ka_upload_widget_process'),
            '#value_callback' => 'ka_upload_widget_value',
            '#upload_validators' => array(),
            // '#input' => TRUE,
            '#theme' => 'ka_upload_widget',
            '#theme_wrappers' => array('form_element'),
        ),
    );
}

/**
 * Implements hook_field_widget_info().
 */
function ka_upload_field_widget_info() {
    return array(
        'ka_upload' => array(
            'label' => t('KA file upload'),
            'field types' => array('file', 'image'),
            'settings' => array(
            ),
            'behaviors' => array(
                'multiple values' => FIELD_BEHAVIOR_CUSTOM,
                'default value' => FIELD_BEHAVIOR_NONE,
            ),
        ),
    );
}

/**
 * Implements hook_field_widget_form().
 */
function ka_upload_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
    $files = array();
    foreach ($items as $delta => $file) {
        if ($file['fid'] != 0) {
            $files[$file['fid']] = $file;
        }
    }
    $element['#default_value'] = $files;

    $element_info = element_info('ka_upload');
    $element += array(
        '#type' => 'ka_upload',
        '#process' => $element_info['#process'],
        '#value_callback' => $element_info['#value_callback'],
        // Allows this field to return an array instead of a single value.
        '#extended' => TRUE,
    );
    // $element['#display_field'] = $field['settings']['display_field'];
    return $element;
}

function ka_upload_widget_process($element, $form_state, $form) {
    $field = field_info_field($element['#field_name']);
    $instance = field_info_instance($element['#entity_type'], $element['#field_name'], $element['#bundle']);

    // Make sure the element's name exists.
    if (!isset($element['#name'])) {
        $element['#name'] = array_shift($element['#parents']);
    }
    $element['#default_value'] = '[]';

    // Set a file upload limit for SWFUpload where 0 is unlimited.
    $limit = ($field['cardinality'] == -1 ? false : $field['cardinality']);

    // Set the button title which is used in the theme function.
    if ($field['type'] == 'image') {
        $element['#button_title'] = format_plural(($limit === 0 ? 2 : $limit), 'Upload new image', 'Upload new images');
    } else {
        $element['#button_title'] = format_plural(($limit === 0 ? 2 : $limit), 'Upload new file', 'Upload new files');
    }
    // Construct the JavaScript settings array.
    $settings['sf_settings'][$element['#id']] = array(
        'module_path' => drupal_get_path('module', 'ka_upload'),
        'upload_url' => url('ka_upload/upload'), // Relative to the jf upload file
        'element_id' => 'ka_element_upload_' . $element['#id'],
        'upload_template_id' => 'upload-' . $element['#id'],
        'download_template_id' => 'download-' . $element['#id'],
        'file_queue_limit' => $limit,
        'existing_file_ids' => ka_get_existing_file_ids($element),
        'post_params' => array(
            'file_path' => $field['settings']['uri_scheme'] . '://' . $instance['settings']['file_directory'],
            'field_name' => $element['#field_name'],
            'element_name' => $element['#name'],
            'instance_settings' => json_encode($instance['settings'] + $field['settings']),
        ),
        'file_size_limit' => ($instance['settings']['max_filesize'] ? (parse_size($instance['settings']['max_filesize']) / 1048576) . 'MB' : 0),
        'file_types' => (empty($instance['settings']['file_extensions']) ? '' : str_replace(" ", "|", $instance['settings']['file_extensions'])),
        'file_types_description' => ($element['#description'] ? $element['#description'] : ''),
        'file_upload_limit' => $limit,
        'custom_settings' => array(
            'upload_stack_value' => (!empty($element['#value'])) ? json_encode($element['#value']) : '[]',
            'max_queue_size' => ($instance['settings']['max_filesize'] ? $instance['settings']['max_filesize'] : 0),
        ),
    );
    drupal_add_js($settings, 'setting');
    ka_upload_load_css();
    ka_upload_load_external_js();
    return $element;
}
function ka_get_existing_file_ids($element){
    $fids = array();
    if(isset($element['#value']) && $element['#value']){
        $files = (array)$element['#value'];
        foreach($files as $file){
            $fids[] = $file['fid'];
        }
    }
    return implode(',', $fids);
}
function ka_upload_widget_value(&$element, $input = FALSE, $form_state = NULL) {

    if (is_string($input)) {
        $input = json_decode($input, TRUE);
    }

    if ($input === FALSE) {
        $default_value = array();

        if (!empty($element['#default_value'])) {
            foreach ($element['#default_value'] as $file) {
                if ($file) {
                    // If we're dealing with an image, create a thumbpath         

                    $default_value[$file['fid']] = $file;
                }
            }
        }
        return $default_value;
    } else {
        // Files need an integer value for the 'display' field.
        if (!empty($input)) {
            foreach ($input as $fid => $file) {
                if (empty($file['display'])) {
                    $input[$fid]['display'] = 0;
                }
            }
        }
        return $input;
    }
}

function theme_ka_upload_widget($variables) {
    $element = $variables['element'];
    $attributes['class'][] = 'form-ka-upload-file';
    // This wrapper is required to apply JS behaviors and CSS styling.
    $output = '';
    $output .= '<div id="ka_element_upload_' . $element['#id'] . '"' . drupal_attributes($attributes) . '>';
    //  $output .= drupal_render_children($element);
    $output .= theme('ka_upload_element', array('element' => $element));
    $output .= '</div>';
    return $output;
}