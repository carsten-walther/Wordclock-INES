//
// Created by Carsten Walther.
//

#include <ESP8266httpUpdate.h>
#include <LittleFS.h>

#include "Updater.h"

Updater updater;

void Updater::requestStart(String filenameIn)
{
    status = 254;
    filename = filenameIn;
    requestFlag = true;
}

void Updater::loop()
{
    if (requestFlag == true)
    {
        requestFlag = false;
        flash(filename);
    }
}

uint8_t Updater::getStatus()
{
    return status;
}

void Updater::flash(String filename)
{
    bool answer = 0;
    File file = LittleFS.open(filename, "r");

    if (!file)
    {
        Serial.println(PSTR("> failed to open file for reading"));
        answer = 0;
    }
    else
    {
        Serial.println(PSTR("> starting update"));

        size_t fileSize = file.size();

        if (!Update.begin(fileSize))
        {
            Serial.println(PSTR("> not enough space for update"));
        }
        else
        {
            Update.writeStream(file);

            if (Update.end())
            {
                Serial.println(PSTR("> successful update"));
                answer = 1;
            }
            else
            {

                Serial.println(PSTR("> error occurred: ") + String(Update.getError()));
            }
        }

        file.close();
    }

    status = answer;
}
