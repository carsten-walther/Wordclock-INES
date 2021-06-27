//
// Created by Carsten Walther.
//

#include <Arduino.h>

#include "main.h"
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

void saveCallback() {
    Serial.println(PSTR("> configuration saved"));
}

void setup()
{
    delay(2000);

    Serial.begin(115200);

    Serial.println(PSTR(" "));
    Serial.println(PSTR("WordClock"));
    Serial.println(PSTR("================================================================================"));

    Serial.print(PSTR("> esp sdk version: "));
    Serial.println(ESP.getSdkVersion());

    Serial.print(PSTR("> esp chip id: "));
    Serial.println(ESP.getChipId());

    Serial.println(PSTR("> starting configuration manager"));
    configurationManager.begin();
    configurationManager.setConfigSaveCallback(saveCallback);
    
    Serial.println(PSTR("> starting wifi manager"));
    wiFiManager.begin(AP_SSID);

    Serial.println(PSTR("> starting web server"));
    webServer.begin();

    if (!wiFiManager.isCaptivePortal())
    {
        Serial.println(PSTR("> starting time sync"));
        
        if (strlen(configurationManager.data.ntpServer) > 7)
        {
            timeSync.begin(configurationManager.data.ntpServer);
        }
        else
        {
            timeSync.begin();
        }

        Serial.print(PSTR("> waiting for time sync result, interval (sec): "));
        Serial.println(configurationManager.data.ntpSyncInterval);
        timeSync.waitForSyncResult(10000);

        if (timeSync.isSynced())
        {
            Serial.println(PSTR("> setting timezone and daylight saving time for time sync"));
            TimeSync::config((int)TIMEZONES[configurationManager.data.timezone] * 3600, configurationManager.data.daylightSavingTime * 3600);

            time_t now = time(nullptr);
            Serial.print(PSTR("> current time is "));
            Serial.print(asctime(localtime(&now)));

            Serial.print(PSTR("> unix timestamp is "));
            Serial.println(TimeSync::getTimestamp());
        }
        else
        {
            Serial.println(PSTR("> timeout while receiving the time"));
        }

        // ... more to come
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
            Serial.println(TimeSync::getTimestamp());

            // ...
        }
    }
}