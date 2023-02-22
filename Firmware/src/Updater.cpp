//
// Created by Carsten Walther.
//

#include <ESP8266httpUpdate.h>

#include "main.h"
#include "Updater.h"

#if defined USE_SPIFFS
#include <FS.h>
#elif defined USE_LITTLEFS
#include <LittleFS.h>
#elif defined USE_SDFS
#include <SDFS.h>
#else
#error Please select a filesystem first by uncommenting one of the "#define USE_xxx" lines at the beginning of the sketch.
#endif

Updater updater;

void Updater::requestStart(String filenameIn)
{
    status = UPDATER_REQUEST_START;
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

uint16_t Updater::getStatus()
{
    return status;
}

void Updater::flash(String filename)
{
#if defined USE_SPIFFS
    FS fileSystem = SPIFFS;
    SPIFFSConfig fileSystemConfig = SPIFFSConfig();
#elif defined USE_LITTLEFS
    FS fileSystem = LittleFS;
    LittleFSConfig fileSystemConfig = LittleFSConfig();
#elif defined USE_SDFS
    FS fileSystem = SDFS;
    SDFSConfig fileSystemConfig = SDFSConfig();
#endif

    // start the SPI Flash File System
    if (fileSystem.begin() != false)
    {
        fileSystemConfig.setAutoFormat(false);
        fileSystem.setConfig(fileSystemConfig);
        DEBUG_PRINTLN(PSTR("> filesystem initialized"));
    }
    else
    {
        DEBUG_PRINTLN(PSTR("> filesystem init failed"));
        status = UPDATER_FILESYSTEM_INIT_FAILED;
    }

    File file = fileSystem.open(filename, "r");

    if (!file)
    {
        DEBUG_PRINTLN(PSTR("> failed to open file for reading"));
        status = UPDATER_FILE_READING_ERROR;
    }
    else
    {
        DEBUG_PRINTLN(PSTR("> starting update"));
        status = UPDATER_UPDATE_START;

        size_t fileSize = file.size();

        if (!Update.begin(fileSize))
        {
            DEBUG_PRINTLN(PSTR("> not enough space for update"));
            status = UPDATER_NOT_ENOUGH_MEMORY;
        }
        else
        {
            Update.writeStream(file);

            if (Update.end())
            {
                DEBUG_PRINTLN(PSTR("> successful update"));
                status = UPDATER_UPDATE_SUCCESSFUL;
            }
            else
            {
                DEBUG_PRINTLN(PSTR("> error occurred: ") + String(Update.getError()));
                status = UPDATER_UNKNOWN_ERROR;
            }
        }

        file.close();
    }
}
