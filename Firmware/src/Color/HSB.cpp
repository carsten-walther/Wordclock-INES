/*
* HSB
*
* @file HSB.cpp
* @package Color
*
* @author Carsten Walther
*/

#include "RGB.h"
#include "HSB.h"

HSB::HSB(RGB color) {

    // convert colors to float between (0.0 - 1.0)
    float r = color.R / 255.0f;
    float g = color.G / 255.0f;
    float b = color.B / 255.0f;

    float max = (r > g && r > b) ? r : (g > b) ? g : b;
    float min = (r < g && r < b) ? r : (g < b) ? g : b;

    float d = max - min;

    float h = 0.0;
    float v = max;
    float s = (v == 0.0f) ? 0 : (d / v);

    if (d != 0.0f) {

        if (r == max) {
            h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        } else if (g == max) {
            h = (b - r) / d + 2.0f;
        } else {
            h = (r - g) / d + 4.0f;
        }

        h /= 6.0f;
    }

#ifdef HSB_FLOAT
    H = h;
    S = s;
    B = v;
#else
    // convert 0.0-1.0 values to 0-255
    H = (uint8_t)(h * 255);
    S = (uint8_t)(s * 255);
    B = (uint8_t)(v * 255);
#endif
}

HSB HSB::LinearBlend(HSB left, HSB right, float progress) {

    return HSB(left.H + ((right.H - left.H) * progress),
        left.S + ((right.S - left.S) * progress),
        left.B + ((right.B - left.B) * progress));
}
