/*
* Settings
*
* @file Settings.h
* @package Settings
*
* @author Carsten Walther
*/

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <Arduino.h>
#include <EEPROM.h>

// Settings Version
#define SETTING_VERSION "Version1.0"

// Parameters Structure
typedef struct {
    // foreground color
    int foregroundColorRed;
    int foregroundColorGreen;
    int foregroundColorBlue;
    // background color
    int backgroundColorRed;
    int backgroundColorGreen;
    int backgroundColorBlue;
    // settings
    int brightness;
    int timeZone;
    bool daylightSavingsTime;
    int sleepHour;
    int sleepMinute;
    int wakeupHour;
    int wakeupMinute;
    int language;
    // version
    char ver[sizeof(SETTING_VERSION)];
} ParametersType;

class Settings
{
    public:
        Settings(ParametersType defaults);
        void save();
        ParametersType *parameters;

    private:
        byte buffer[sizeof(ParametersType)];
};

#endif  // SETTINGS_H_
