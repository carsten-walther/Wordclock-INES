/*
 * Timezones
 *
 * @file Timezones.h
 * @package .
 *
 * @author Carsten Walther
 */

#ifndef TIMEZONES_H_
#define TIMEZONES_H_

#include <avr/pgmspace.h>

extern const double TIMEZONES[] PROGMEM;
const double TIMEZONES[] = {
        -12, // 0
        -11,
        -10,
        -9,
        -8,
        -8,
        -7,
        -7,
        -7,
        -6,
        -6, // 10
        -6,
        -6,
        -5,
        -5,
        -5,
        -4,
        -4,
        -4,
        -4,
        -3.5, // 20
        -3,
        -3,
        -3,
        -3,
        -2,
        -1,
        -1,
        0,
        0,
        1, // 30
        1,
        1,
        1,
        1,
        2,
        2,
        2,
        2,
        2,
        2, // 40
        2,
        2,
        2,
        3,
        3,
        3,
        3,
        3.5,
        4,
        4, // 50
        4,
        4.5,
        5,
        5,
        5.5,
        5.5,
        5.75,
        6,
        6,
        6.5, // 60
        7,
        7,
        8,
        8,
        8,
        8,
        8,
        9,
        9,
        9, // 70
        9.5,
        9.5,
        10,
        10,
        10,
        10,
        10,
        11,
        12,
        12, // 80
        13
};

#endif // TIMEZONES_H_
