/*
* HSB
*
* @file HSB.h
* @package Color
*
* @author Carsten Walther
*/

#ifndef HSB_H_
#define HSB_H_

#include <Arduino.h>

#define HSB_FLOAT (true)

// HSB represents a color object that is represented by Hue, Saturation,
// Brightness component values. It contains helpful color routines to
// manipulate the color.
struct HSB {

#ifdef HSB_FLOAT
    // Construct a HSB using H, S, B values (0.0 - 1.0)
    HSB(float h, float s, float b) : H(h), S(s), B(b) {

    };
#else
    // Construct a HSB using H, S, B values (0 - 255)
    HSB(uint8_t h, uint8_t s, uint8_t b) : H(h), S(s), B(b) {

    };
#endif

    // Construct a HSB using RgbColor
    HSB(RGB color);

    // Construct a HSB that will have its values set in latter operations
    // CAUTION:  The H,S,B members are not initialized and may not be consistent
    HSB() {

    };

    // LinearBlend between two colors by the amount defined by progress variable
    // left - the color to start the blend at
    // right - the color to end the blend at
    // progress - (0.0 - 1.0) value
    //   where 0.0 will return left and 1.0 will return right
    //   and a value between will blend the color weighted linearly between them
    static HSB LinearBlend(HSB left, HSB right, float progress);

    // Hue, Saturation, Lightness color members
#ifdef HSB_FLOAT
    float H;
    float S;
    float B;
#else
    uint8_t H;
    uint8_t S;
    uint8_t B;
#endif
};

#endif  // HSB_H_
