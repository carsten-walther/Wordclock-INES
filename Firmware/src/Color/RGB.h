/*
* RGB
*
* @file RGB.h
* @package Color
*
* @author Carsten Walther
*/

#ifndef RGB_H_
#define RGB_H_

#include <Arduino.h>

struct HSL;
struct HSB;

// RGB represents a color object that is represented by Red, Green, Blue
// component values.  It contains helpful color routines to manipulate the
// color.
struct RGB
{
    // Construct a RGB using R, G, B values (0-255)
    RGB(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) {

    };

    // Construct a RGB using a single brightness value (0-255)
    // This works well for creating gray tone colors
    // (0) = blakc, (255) = white, (128) = gray
    RGB(uint8_t brightness) : R(brightness), G(brightness), B(brightness) {

    };

    // Construct a RGB using HSL
    RGB(HSL color);

    // Construct a RGB using HSB
    RGB(HSB color);

    // Construct a RGB that will have its values set in latter operations
    // CAUTION:  The R,G,B members are not initialized and may not be consistent
    RGB() {

    };

    // CalculateBrightness will calculate the overall brightness
    // NOTE: This is a simple linear brightness
    uint8_t CalculateBrightness() const;

    // Darken will adjust the color by the given delta toward black
    // NOTE: This is a simple linear change
    // delta - (0-255) the amount to dim the color
    void Darken(uint8_t delta);

    // Lighten will adjust the color by the given delta toward white
    // NOTE: This is a simple linear change
    // delta - (0-255) the amount to lighten the color
    void Lighten(uint8_t delta);

    // LinearBlend between two colors by the amount defined by progress variable
    // left - the color to start the blend at
    // right - the color to end the blend at
    // progress - (0.0 - 1.0) value
    //   where 0 will return left and 1.0 will return right
    //   and a value between will blend the color weighted linearly between them
    static RGB LinearBlend(RGB left, RGB right, float progress);

    // Red, Green, Blue color members (0-255) where
    // (0,0,0) is black and (255,255,255) is white
    uint8_t R;
    uint8_t G;
    uint8_t B;
};

#endif  // RGB_H_
