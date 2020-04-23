/*
* RGB
*
* @file RGB.cpp
* @package Color
*
* @author Carsten Walther
*/

#include "RGB.h"
#include "HSL.h"
#include "HSB.h"

static float _CalcColor(float p, float q, float t) {

    if (t < 0.0f) {
        t += 1.0f;
    }

    if (t > 1.0f) {
        t -= 1.0f;
    }

    if (t < 1.0f / 6.0f) {
        return p + (q - p) * 6.0f * t;
    }

    if (t < 0.5f) {
        return q;
    }

    if (t < 2.0f / 3.0f) {
        return p + ((q - p) * (2.0f / 3.0f - t) * 6.0f);
    }

    return p;
}

RGB::RGB(HSL color) {

    float r;
    float g;
    float b;
#ifdef HSL_FLOAT
    float h = color.H;
    float s = color.S;
    float l = color.L;
#else
    float h = color.H / 255.0f;
    float s = color.S / 255.0f;
    float l = color.L / 255.0f;
#endif

    if (color.S == 0.0f || color.L == 0.0f) {
        // achromatic or black
        r = g = b = l;
    } else {
        float q = l < 0.5f ? l * (1.0f + s) : l + s - (l * s);
        float p = 2.0f * l - q;

        r = _CalcColor(p, q, h + 1.0f / 3.0f);
        g = _CalcColor(p, q, h);
        b = _CalcColor(p, q, h - 1.0f / 3.0f);
    }

    R = (uint8_t)(r * 255.0f);
    G = (uint8_t)(g * 255.0f);
    B = (uint8_t)(b * 255.0f);
}

RGB::RGB(HSB color) {

    float r;
    float g;
    float b;
#ifdef HSB_FLOAT
    float h = color.H;
    float s = color.S;
    float v = color.B;
#else
    float h = color.H / 255.0f;
    float s = color.S / 255.0f;
    float v = color.B / 255.0f;
#endif

    if (color.S == 0.0f) {
        // achromatic or black
        r = g = b = v;
    } else {
        if (h < 0.0f) {
            h += 1.0f;
        }

        if (h > 1.0f) {
            h -= 1.0f;
        }

        h *= 6.0f;

        int i = (int)h;

        float f = h - i;
        float q = v * (1.0f - s * f);
        float p = v * (1.0f - s);
        float t = v * (1.0f - s * (1.0f - f));

        switch (i) {
            case 0:
                r = v;
                g = t;
                b = p;
                break;

            case 1:
                r = q;
                g = v;
                b = p;
                break;

            case 2:
                r = p;
                g = v;
                b = t;
                break;

            case 3:
                r = p;
                g = q;
                b = v;
                break;

            case 4:
                r = t;
                g = p;
                b = v;
                break;

            default:
                r = v;
                g = p;
                b = q;
                break;
        }
    }

    R = (uint8_t)(r * 255.0f);
    G = (uint8_t)(g * 255.0f);
    B = (uint8_t)(b * 255.0f);
}

uint8_t RGB::CalculateBrightness() const {

    return (uint8_t)(((uint16_t)R + (uint16_t)G + (uint16_t)B) / 3);
}

void RGB::Darken(uint8_t delta) {

    if (R > delta) {
        R -= delta;
    } else {
        R = 0;
    }

    if (G > delta) {
        G -= delta;
    } else {
        G = 0;
    }

    if (B > delta) {
        B -= delta;
    } else {
        B = 0;
    }
}

void RGB::Lighten(uint8_t delta) {

    if (R < 255 - delta) {
        R += delta;
    } else {
        R = 255;
    }

    if (G < 255 - delta) {
        G += delta;
    } else {
        G = 255;
    }

    if (B < 255 - delta) {
        B += delta;
    } else {
        B = 255;
    }
}

RGB RGB::LinearBlend(RGB left, RGB right, float progress) {

    return RGB( left.R + ((right.R - left.R) * progress),
        left.G + ((right.G - left.G) * progress),
        left.B + ((right.B - left.B) * progress));
}
