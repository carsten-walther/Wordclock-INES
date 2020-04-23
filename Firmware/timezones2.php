<?php

$timezone_abbreviations = DateTimeZone::listAbbreviations();

#echo '<pre>' . print_r($timezone_abbreviations, true) . '</pre>';

$timezones = [];

foreach ($timezone_abbreviations as $values) {
    $subtimezones = [];
    foreach ($values as $key => $value) {
        $subtimezones[$value['offset']] = $value['timezone_id'];
    }
    $timezones = array_merge($timezones, $subtimezones);
    #$timezones[] = $subtimezones;
}

echo '<pre>' . print_r($timezones, true) . '</pre>';
