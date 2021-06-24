//
// Created by Carsten Walther.
//

#include <EEPROM.h>
#include <Arduino.h>

#include "ConfigurationManager.h"

ConfigurationManager configurationManager;

bool ConfigurationManager::begin(int numBytes)
{
    EEPROM.begin(numBytes);

    uint32_t storedVersion;
    uint8_t checksumData = 0;
    uint8_t checksumInternal = 0;

    EEPROM.get(0, internal);
    EEPROM.get(SIZE_INTERNAL, checksumInternal);
    EEPROM.get(SIZE_INTERNAL + 1, storedVersion);
    EEPROM.get(SIZE_INTERNAL + 5, data);
    EEPROM.get(SIZE_INTERNAL + 5 + sizeof(data), checksumData);

    bool returnValue = true;

    // reset configuration data if checksum mismatch
    if (checksumData != checksum(reinterpret_cast<uint8_t *>(&data), sizeof(data)) || storedVersion != version)
    {
        Serial.println(PSTR("> config data checksum mismatch"));
        reset();
        returnValue = false;
    }

    // reset internal data if checksum mismatch
    if (checksumInternal != checksum(reinterpret_cast<uint8_t *>(&internal), sizeof(internal)))
    {
        Serial.println(PSTR("> internal data checksum mismatch"));
        internal = internalData();
        requestSave = true;
        returnValue = false;
    }

    return returnValue;
}

void ConfigurationManager::reset()
{
    memcpy_P(&data, &defaults, sizeof(data));
    requestSave = true;
}

void ConfigurationManager::saveRaw(uint8_t bytes[])
{
    memcpy(&data, bytes, sizeof(data));
    requestSave = true;
}

void ConfigurationManager::saveExternal(Configuration *extData)
{
    memcpy(&data, extData, sizeof(data));
    requestSave = true;
}

void ConfigurationManager::save()
{
    EEPROM.put(0, internal);

    // save checksum for internal data
    EEPROM.put(SIZE_INTERNAL, checksum(reinterpret_cast<uint8_t *>(&internal), sizeof(internal)));

    EEPROM.put(SIZE_INTERNAL + 1, version);
    EEPROM.put(SIZE_INTERNAL + 5, data);

    // save checksum for configuration data
    EEPROM.put(SIZE_INTERNAL + 5 + sizeof(data), checksum(reinterpret_cast<uint8_t *>(&data), sizeof(data)));

    EEPROM.commit();

    if (_configsavecallback != NULL)
    {
        _configsavecallback();
    }
}

void ConfigurationManager::setConfigSaveCallback(std::function<void()> func)
{
    _configsavecallback = func;
}

void ConfigurationManager::loop()
{
    if (requestSave)
    {
        requestSave = false;
        save();
    }
}

uint8_t ConfigurationManager::checksum(uint8_t *byteArray, unsigned long length)
{
    uint8_t value = 0;
    unsigned long counter;

    for (counter = 0; counter < length; counter++)
    {
        value += *byteArray;
        byteArray++;
    }

    return (uint8_t)(256 - value);
}