<?php

$regions = [
    'Africa' => DateTimeZone::AFRICA,
    'America' => DateTimeZone::AMERICA,
    'Antarctica' => DateTimeZone::ANTARCTICA,
    'Asia' => DateTimeZone::ASIA,
    'Atlantic' => DateTimeZone::ATLANTIC,
    'Australia' => DateTimeZone::AUSTRALIA,
    'Europe' => DateTimeZone::EUROPE,
    'Indian' => DateTimeZone::INDIAN,
    'Pacific' => DateTimeZone::PACIFIC
];

$timezones = [];

foreach ($regions as $name => $mask) {

    $zones = DateTimeZone::listIdentifiers($mask);

    foreach ($zones as $timezone) {
        $tz = new DateTimeZone($timezone);
        // Lets sample the time there right now
        $time = new DateTime(NULL, $tz);
        // Us dumb Americans can't handle millitary time
        $ampm = $time->format('H') > 12 ? ' ('. $time->format('g:i a'). ')' : '';
        // calc offset
        $offset = ($time->format('Z') / 3600);
            // Remove region name and add a sample time
        $timezones[$name][$timezone] = [
            'offset' => $offset,
            'label' => str_replace('_', ' ', substr($timezone, strlen($name) + 1)) . ' (' . ($offset > 0 ? ('+' . $offset) : $offset) . 'h)'
        ];
    }
}

// View

print '<select id="timezone">';
foreach ($timezones as $region => $listItems) {
    print '<optgroup label="' . $region . '">';
    foreach ($listItems as $name => $listItem) {
        print '<option value="' . $listItem['offset'] . '" name="' . $name . '">' . $listItem['label'] . '</option>';
    }
    print '</optgroup>';
}
print '</select>';
