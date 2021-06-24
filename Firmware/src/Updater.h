//
// Created by Carsten Walther.
//

#ifndef WORDCLOCK_UPDATER_H
#define WORDCLOCK_UPDATER_H

#include <Arduino.h>

class Updater {
private:
    String filename;
    bool requestFlag = false;
    uint8_t status = 255;

    void flash(String filename);

public:
    void requestStart(String filename);
    void loop();
    uint8_t getStatus();
};

extern Updater updater;

#endif // WORDCLOCK_UPDATER_H
