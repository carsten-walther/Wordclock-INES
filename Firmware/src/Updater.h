//
// Created by Carsten Walther.
//

#ifndef WORDCLOCK_UPDATER_H
#define WORDCLOCK_UPDATER_H

#include <Arduino.h>

#define UPDATER_UPDATE_SUCCESSFUL        1
#define UPDATER_UNKNOWN_ERROR           16
#define UPDATER_NOT_ENOUGH_MEMORY       32
#define UPDATER_FILE_READING_ERROR      64
#define UPDATER_UPDATE_START           128
#define UPDATER_REQUEST_START          256
#define UPDATER_FILESYSTEM_INIT_FAILED 512

class Updater {
private:
    String filename;
    bool requestFlag = false;
    uint16_t status = UPDATER_REQUEST_START;

    void flash(String filename);

public:
    void requestStart(String filename);
    void loop();
    uint16_t getStatus();
};

extern Updater updater;

#endif // WORDCLOCK_UPDATER_H
