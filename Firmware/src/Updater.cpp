//
// Created by Carsten Walther.
//

#include <ESP8266httpUpdate.h>
#include <LittleFS.h>

#include "main.h"
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
        DEBUG_PRINTLN(PSTR("> failed to open file for reading"));
        answer = 0;
    }
    else
    {
        DEBUG_PRINTLN(PSTR("> starting update"));

        size_t fileSize = file.size();

        if (!Update.begin(fileSize))
        {
            DEBUG_PRINTLN(PSTR("> not enough space for update"));
        }
        else
        {
            Update.writeStream(file);

            if (Update.end())
            {
                DEBUG_PRINTLN(PSTR("> successful update"));
                answer = 1;
            }
            else
            {

                DEBUG_PRINTLN(PSTR("> error occurred: ") + String(Update.getError()));
            }
        }

        file.close();
    }

    status = answer;
}
