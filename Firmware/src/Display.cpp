//
// Created by Carsten Walther.
//

#include "Display.h"
#include "ConfigurationManager.h"

Display display;

void Display::begin(uint16_t number, uint16_t pin)
{
    // init WS2811 PIN
    pinMode(pin, OUTPUT);

    // Parameter 1 = number of pixels in strip
    // Parameter 2 = Arduino pin number (most are valid)
    // Parameter 3 = pixel type flags, add together as needed:
    // NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
    // NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
    // NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
    // NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
    ledStrip = Adafruit_NeoPixel(number, pin, NEO_GRB + NEO_KHZ800);

    ledStrip.begin();
    ledStrip.setBrightness(0);
    ledStrip.clear();
    ledStrip.show();
}

void Display::showScanner()
{
    unsigned int speedDelay = 125;

    if (millis() - previousMillis >= speedDelay) {

        previousMillis = millis();

        // set brightness
        ledStrip.setBrightness(configurationManager.data.brightness);

        // RightToLeft
        for (int i = 12; i >= 9; i--) {

            for (int i = 0; i < ledStrip.numPixels(); i++) {
                ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
            }

            ledStrip.setPixelColor(i, ledStrip.Color(255, 0, 0));
            ledStrip.show();

            delay(speedDelay);
        }
        delay(speedDelay / 2);

        // LeftToRight
        for (int i = 9; i <= 12; i++) {

            for (int i = 0; i < ledStrip.numPixels(); i++) {
                ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
            }

            ledStrip.setPixelColor(i, ledStrip.Color(255, 0, 0));
            ledStrip.show();

            delay(speedDelay);
        }
        delay(speedDelay / 2);
    }
}

void Display::showTest()
{
    unsigned int speedDelay = 125;

    if (millis() - previousMillis >= speedDelay) {

        previousMillis = millis();

        // set brightness
        ledStrip.setBrightness(configurationManager.data.brightness);

        for (int i = 0; i < ledStrip.numPixels(); i++) {
            ledStrip.setPixelColor(i, ledStrip.Color(255, 255, 255));
            delay(speedDelay);
            ledStrip.show();
        }

        for (int i = 0; i < ledStrip.numPixels(); i++) {
            ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
            delay(speedDelay);
            ledStrip.show();
        }
    }
}

void Display::showNothing()
{
    if (millis() - previousMillis >= 1000) {

        previousMillis = millis();

        ledStrip.setBrightness(0);
        ledStrip.clear();
        ledStrip.show();
    }
}

void Display::showTime(uint16_t hours, uint16_t minutes)
{
    if (millis() - previousMillis >= 1000) {

        previousMillis = millis();

        while (hours < 0) {
            hours += 12;
        }

        while (hours > 24) {
            hours -= 12;
        }

        // set colors
        uint32_t foregroundCol = ledStrip.Color(configurationManager.data.foregroundRed, configurationManager.data.foregroundGreen, configurationManager.data.foregroundBlue);
        uint32_t backgroundCol = ledStrip.Color(configurationManager.data.backgroundRed, configurationManager.data.backgroundGreen, configurationManager.data.backgroundBlue);

        // set brightness
        ledStrip.setBrightness(configurationManager.data.brightness);

        for (int i = 0; i < ledStrip.numPixels(); i++) {
            ledStrip.setPixelColor(i, backgroundCol);
        }

        // minutes
        switch (minutes / 5) {
            case 0:
                // glatte Stunde
                ledStrip.setPixelColor(DE_GENAU, foregroundCol);
                break;
            case 1:
                // 5 nach
                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_NACH, foregroundCol);
                break;
            case 2:
                // 10 nach
                ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                ledStrip.setPixelColor(DE_NACH, foregroundCol);
                break;
            case 3:
                if ((configurationManager.data.language == LANGUAGE_DE_SW) || (configurationManager.data.language == LANGUAGE_DE_SA)) {
                    // viertel
                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    hours = hours + 1;
                } else {
                    // viertel nach
                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    ledStrip.setPixelColor(DE_NACH, foregroundCol);
                }
                break;
            case 4:
                if (configurationManager.data.language == LANGUAGE_DE_SA) {
                    // 10 vor halb
                    ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                    ledStrip.setPixelColor(DE_VOR, foregroundCol);
                    ledStrip.setPixelColor(DE_HALB, foregroundCol);
                    hours = hours + 1;
                } else {
                    // 20 nach
                    ledStrip.setPixelColor(DE_ZWANZIG, foregroundCol);
                    ledStrip.setPixelColor(DE_NACH, foregroundCol);
                }
                break;
            case 5:
                // 5 vor halb
                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_VOR, foregroundCol);
                ledStrip.setPixelColor(DE_HALB, foregroundCol);
                hours = hours + 1;
                break;
            case 6:
                // halb
                ledStrip.setPixelColor(DE_GENAU, foregroundCol);
                ledStrip.setPixelColor(DE_HALB, foregroundCol);
                hours = hours + 1;
                break;
            case 7:
                // 5 nach halb
                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_NACH, foregroundCol);
                ledStrip.setPixelColor(DE_HALB, foregroundCol);
                hours = hours + 1;
                break;
            case 8:
                if (configurationManager.data.language == LANGUAGE_DE_SA) {
                    // 10 nach halb
                    ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                    ledStrip.setPixelColor(DE_NACH, foregroundCol);
                    ledStrip.setPixelColor(DE_HALB, foregroundCol);
                    hours = hours + 1;
                } else {
                    // 20 vor
                    ledStrip.setPixelColor(DE_ZWANZIG, foregroundCol);
                    ledStrip.setPixelColor(DE_VOR, foregroundCol);
                }
                break;
            case 9:
                if ((configurationManager.data.language == LANGUAGE_DE_SW) || (configurationManager.data.language == LANGUAGE_DE_BA) || (configurationManager.data.language == LANGUAGE_DE_SA)) {
                    // drei viertel
                    ledStrip.setPixelColor(DE_DREI, foregroundCol);
                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    hours = hours + 1;
                } else {
                    // viertel vor
                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    ledStrip.setPixelColor(DE_VOR, foregroundCol);
                    hours = hours + 1;
                }
                break;
            case 10:
                // 10 vor
                ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                ledStrip.setPixelColor(DE_VOR, foregroundCol);
                hours = hours + 1;
                break;
            case 11:
                // 5 vor
                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_VOR, foregroundCol);
                hours = hours + 1;
                break;
        }

        // hours
        switch (hours) {
            case 0:
            case 12:
            case 24:
                ledStrip.setPixelColor(DE_H_ZWOELF, foregroundCol);
                break;
            case 1:
            case 13:
                ledStrip.setPixelColor(DE_H_EINS, foregroundCol);
                break;
            case 2:
            case 14:
                ledStrip.setPixelColor(DE_H_ZWEI, foregroundCol);
                break;
            case 3:
            case 15:
                ledStrip.setPixelColor(DE_H_DREI, foregroundCol);
                break;
            case 4:
            case 16:
                ledStrip.setPixelColor(DE_H_VIER, foregroundCol);
                break;
            case 5:
            case 17:
                ledStrip.setPixelColor(DE_H_FUENF, foregroundCol);
                break;
            case 6:
            case 18:
                ledStrip.setPixelColor(DE_H_SECHS, foregroundCol);
                break;
            case 7:
            case 19:
                ledStrip.setPixelColor(DE_H_SIEBEN, foregroundCol);
                break;
            case 8:
            case 20:
                ledStrip.setPixelColor(DE_H_ACHT, foregroundCol);
                break;
            case 9:
            case 21:
                ledStrip.setPixelColor(DE_H_NEUN, foregroundCol);
                break;
            case 10:
            case 22:
                ledStrip.setPixelColor(DE_H_ZEHN, foregroundCol);
                break;
            case 11:
            case 23:
                ledStrip.setPixelColor(DE_H_ELF, foregroundCol);
                break;
        }

        // minutes indicators
        switch (minutes % 5) {
            case 0:
                break;
            case 1:
                ledStrip.setPixelColor(DE_M_EINS, foregroundCol);
                break;
            case 2:
                ledStrip.setPixelColor(DE_M_EINS, foregroundCol);
                ledStrip.setPixelColor(DE_M_ZWEI, foregroundCol);
                break;
            case 3:
                ledStrip.setPixelColor(DE_M_EINS, foregroundCol);
                ledStrip.setPixelColor(DE_M_ZWEI, foregroundCol);
                ledStrip.setPixelColor(DE_M_DREI, foregroundCol);
                break;
            case 4:
                ledStrip.setPixelColor(DE_M_EINS, foregroundCol);
                ledStrip.setPixelColor(DE_M_ZWEI, foregroundCol);
                ledStrip.setPixelColor(DE_M_DREI, foregroundCol);
                ledStrip.setPixelColor(DE_M_VIER, foregroundCol);
                break;
        }

        ledStrip.show();
    }
}