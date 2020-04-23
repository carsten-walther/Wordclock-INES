/*
* HSL
*
* @file HSL.cpp
* @package Color
*
* @author Carsten Walther
*/

#include "RGB.h"
#include "HSL.h"

HSL::HSL(RGB color) {

    // convert colors to float between (0.0 - 1.0)
    float r = color.R / 255.0f;
    float g = color.G / 255.0f;
    float b = color.B / 255.0f;

    float max = (r > g && r > b) ? r : (g > b) ? g : b;
    float min = (r < g && r < b) ? r : (g < b) ? g : b;

    float h, s, l;

    l = (max + min) / 2.0f;

    if (max == min) {
        h = s = 0.0f;
    } else {
        float d = max - min;
        s = (l > 0.5f) ? d / (2.0f - (max + min)) : d / (max + min);

        if (r > g && r > b) {
            h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        } else if (g > b) {
            h = (b - r) / d + 2.0f;
        } else {
            h = (r - g) / d + 4.0f;
        }

        h /= 6.0f;
    }

#ifdef HSL_FLOAT
    H = h;
    S = s;
    L = l;
#else
    // convert 0.0-1.0 values to 0-255
    H = (uint8_t)(h * 255);
    S = (uint8_t)(s * 255);
    L = (uint8_t)(l * 255);
#endif
}

HSL HSL::LinearBlend(HSL left, HSL right, float progress) {

    return HSL(left.H + ((right.H - left.H) * progress),
        left.S + ((right.S - left.S) * progress),
        left.L + ((right.L - left.L) * progress));
}
