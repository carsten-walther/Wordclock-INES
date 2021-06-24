//
// Created by Carsten Walther.
//

#ifndef WORDCLOCK_CONFIGURATIONMANAGER_H
#define WORDCLOCK_CONFIGURATIONMANAGER_H

#include <Arduino.h>

#include "Configuration.h"

//
// allocate 32 bytes to have room for future expansion
//
#define SIZE_INTERNAL 32

struct internalData
{
    uint32_t ip;
    uint32_t gw;
    uint32_t sub;
    uint32_t dns;
};

class ConfigurationManager
{
private:
    bool requestSave = false;
    
    uint8_t checksum(uint8_t *byteArray, unsigned long length);
    std::function<void()> _configsavecallback;

public:
    internalData internal;
    Configuration data;

    bool begin(int numBytes = 512);
    void saveRaw(uint8_t bytes[]);
    void saveExternal(Configuration *extData);
    void save();
    void reset();
    void loop();
    void setConfigSaveCallback(std::function<void()> func);
};

extern ConfigurationManager configurationManager;

#endif // WORDCLOCK_CONFIGURATIONMANAGER_H
