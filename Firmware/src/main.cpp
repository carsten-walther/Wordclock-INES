//
// Created by Carsten Walther.
//

#include <Arduino.h>

#include "main.h"
#include "Display.h"
#include "Updater.h"
#include "TimeSync.h"
#include "Timezones.h"
#include "WebServer.h"
#include "WiFiManager.h"
#include "ConfigurationManager.h"

struct task
{
    unsigned long rate;
    unsigned long previous;
};

task taskA = {
    .rate = 1000,
    .previous = 0
};

// clockMode
int clockMode = CLOCKMODE_NORMAL;

void saveCallback() {
    DEBUG_PRINTLN(PSTR("> configuration saved"));
}

void setup()
{
    #ifdef DEBUG
    Serial.begin(115200);
    #endif

    DEBUG_PRINTLN(PSTR("\n"));
    DEBUG_PRINT(PSTR("WordClock - "));
    DEBUG_PRINTLN(VERSION);
    DEBUG_PRINTLN(PSTR("================================================================================"));

    DEBUG_PRINT(PSTR("> esp sdk version: "));
    DEBUG_PRINTLN(ESP.getSdkVersion());

    DEBUG_PRINT(PSTR("> esp chip id: "));
    DEBUG_PRINTLN(ESP.getChipId());

    DEBUG_PRINTLN(PSTR("> starting display"));
    display.begin(WS2811_NUMBER, WS2811_DATA_PIN);

    DEBUG_PRINTLN(PSTR("> starting configuration manager"));
    configurationManager.begin();
    configurationManager.setConfigSaveCallback(saveCallback);
    
    DEBUG_PRINTLN(PSTR("> starting wifi manager"));
    wiFiManager.begin(AP_SSID, 60000);

    DEBUG_PRINTLN(PSTR("> starting web server"));
    webServer.begin();

    if (!wiFiManager.isCaptivePortal())
    {
        DEBUG_PRINTLN(PSTR("> starting time sync"));
        
        if (strlen(configurationManager.data.ntpServer) > 7)
        {
            timeSync.begin(configurationManager.data.ntpServer);
        }
        else
        {
            timeSync.begin();
        }

        DEBUG_PRINT(PSTR("> waiting for time sync result, interval (sec): "));
        DEBUG_PRINTLN(configurationManager.data.ntpSyncInterval);
        timeSync.waitForSyncResult(10000);

        if (timeSync.isSynced())
        {
            DEBUG_PRINTLN(PSTR("> setting timezone and daylight saving time for time sync"));
            TimeSync::config((int)TIMEZONES[configurationManager.data.timezone] * 3600, configurationManager.data.daylightSavingTime * 3600);

            time_t now = time(nullptr);
            DEBUG_PRINT(PSTR("> current time is "));
            DEBUG_PRINT(asctime(localtime(&now)));

            DEBUG_PRINT(PSTR("> unix timestamp is "));
            DEBUG_PRINTLN(TimeSync::getTimestamp());
        }
        else
        {
            DEBUG_PRINTLN(PSTR("> timeout while receiving the time"));
        }
    }
}

void loop()
{
    configurationManager.loop();
    wiFiManager.loop();
    webServer.loop();
    updater.loop();

    if (!wiFiManager.isCaptivePortal())
    {
        if (taskA.previous == 0 || (millis() - taskA.previous > taskA.rate))
        {
            taskA.previous = millis();

            //do task
            #ifdef DEBUG
            time_t now = time(nullptr);
            #endif
            DEBUG_PRINT(PSTR("> current time is: "));
            DEBUG_PRINT(asctime(localtime(&now)));

            // Set default clockMode
            clockMode = CLOCKMODE_NORMAL;

            // if time >= sleepTime or time <= wakeupTime
            if ((timeSync.getHours() == configurationManager.data.sleepHour && timeSync.getMinutes() >= configurationManager.data.sleepMinute) || (timeSync.getHours() == configurationManager.data.wakeupHour && timeSync.getMinutes() < configurationManager.data.wakeupMinute)) {
                clockMode = CLOCKMODE_NIGHT;
            }

            // if sleepTime < wakeupTime and hour > sleepHour and hour < wakeupHour
            if (configurationManager.data.sleepHour < configurationManager.data.wakeupHour && timeSync.getHours() >= configurationManager.data.sleepHour && timeSync.getHours() <= configurationManager.data.wakeupHour) {
                clockMode = CLOCKMODE_NIGHT;
            }

            // if sleepTime > wakeupTime and hour > sleepHour or hour < wakeupHour
            if (configurationManager.data.sleepHour > configurationManager.data.wakeupHour && (timeSync.getHours() >= configurationManager.data.sleepHour || timeSync.getHours() <= configurationManager.data.wakeupHour)) {
                clockMode = CLOCKMODE_NIGHT;
            }

            // override clockMode
            if (configurationManager.data.mode > -1) {
                clockMode = configurationManager.data.mode;
            }

            DEBUG_PRINT(PSTR("> clockmode: "));

            // update face
            switch (clockMode)
            {
                case CLOCKMODE_NORMAL:
                    DEBUG_PRINTLN("time mode");
                    display.showTime(timeSync.getHours(), timeSync.getMinutes());
                    break;
                case CLOCKMODE_NIGHT:
                    DEBUG_PRINTLN("night mode");
                    display.showNothing();
                    break;
                case CLOCKMODE_SCANNER:
                    DEBUG_PRINTLN("scanner mode");
                    display.showScanner();
                    break;
                case CLOCKMODE_TEST:
                    DEBUG_PRINTLN("test mode");
                    display.showTest();
                    break;
            }
        }
    }
}