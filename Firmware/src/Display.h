//
// Created by Carsten Walther.
//

#ifndef WORDCLOCK_DISPLAY_H
#define WORDCLOCK_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "main.h"

class Display
{
public:
    void begin(uint16_t number, uint16_t pin);

    void showScanner();
    void showTest();
    void showNothing();
    void showTime(uint16_t hours, uint16_t minutes);

private:
    Adafruit_NeoPixel ledStrip;
    unsigned long previousMillis = 0;
};

extern Display display;

#endif // WORDCLOCK_DISPLAY_H