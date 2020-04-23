/*
* HSL
*
* @file HSL.h
* @package Color
*
* @author Carsten Walther
*/

#ifndef HSL_H_
#define HSL_H_

#include <Arduino.h>

#define HSL_FLOAT (true)

// HSL represents a color object that is represented by Hue,
// Saturation, Lightness component values. It contains helpful color routines
// to manipulate the color.
struct HSL {

#ifdef HSL_FLOAT
    // Construct a HSL using H, S, L values (0.0 - 1.0)
    HSL(float h, float s, float l) : H(h), S(s), L(l) {

    };
#else
    // Construct a HSL using H, S, L values (0 - 255)
    // generally, L should be limited to between (0-127)
    HSL(uint8_t h, uint8_t s, uint8_t l) : H(h), S(s), L(l) {

    };
#endif

    // Construct a HSL using RGB
    HSL(RGB color);

    // Construct a HSL that will have its values set in latter operations
    // CAUTION:  The H,S,L members are not initialized and may not be consistent
    HSL() {

    };

    // LinearBlend between two colors by the amount defined by progress variable
    // left - the color to start the blend at
    // right - the color to end the blend at
    // progress - (0.0 - 1.0) value
    //   where 0.0 will return left and 1.0 will return right and a value
    //   between will blend the color weighted linearly between them
    static HSL LinearBlend(HSL left, HSL right, float progress);

    // Hue, Saturation, Lightness color members
#ifdef HSL_FLOAT
    float H;
    float S;
    float L;
#else
    uint8_t H;
    uint8_t S;
    uint8_t L;
#endif
};

#endif  // HSL_H_
