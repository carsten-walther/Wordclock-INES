/*
* Settings
*
* @file Settings.cpp
* @package Settings
*
* @author Carsten Walther
*/

#include <Arduino.h>
#include "Settings.h"

/**
 * Constructor
 *
 * @param ParametersType defaults
 *
 * @return void
 */
Settings::Settings(ParametersType defaults)
{
    memcpy(buffer, &defaults, sizeof(defaults));

    EEPROM.begin(4095);

    // Read version number from EEPROM
    char savedVersion[sizeof(SETTING_VERSION)];
    int eepromVersionOffset = sizeof(buffer) - (sizeof(SETTING_VERSION) + 1);
    EEPROM.get(eepromVersionOffset, savedVersion);

    // If version in EEPROM is correct,
    // overwrite the default config with eeprom values
    if (strcmp(savedVersion, SETTING_VERSION) == 0) {
        // Overwrite the structure with EEPROM data
        EEPROM.get(cfgStart, buffer);
    }

    EEPROM.end();

    parameters = (ParametersType *) buffer;
}

/**
 * save
 *
 * @return void
 */
void Settings::save()
{
    EEPROM.begin(4095);
    EEPROM.put(cfgStart, buffer);

    delay(250);

    EEPROM.commit();
    EEPROM.end();
}
