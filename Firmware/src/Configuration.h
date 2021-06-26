//
// Created by Carsten Walther.
//

#ifndef WORDCLOCK_CONFIGURATION_H
#define WORDCLOCK_CONFIGURATION_H

#include "main.h"

struct Configuration
{
    // foreground color
    int foregroundRed;
    int foregroundGreen;
    int foregroundBlue;

    // background color
    int backgroundRed;
    int backgroundGreen;
    int backgroundBlue;

    // settings
    int mode;
    int brightness;
    int sleepHour;
    int sleepMinute;
    int wakeupHour;
    int wakeupMinute;
    int language;
    int timezone;
    bool daylightSavingTime;
    int ntpSyncInterval;
    char ntpServer[64];

    // network
    char ssid[32];
    char pass[32];
    char hostname[32];

    // access
    bool useAuth;
    char authUsername[32];
    char authPassword[32];
};

extern unsigned long version;
extern const Configuration defaults;

#endif // WORDCLOCK_CONFIGURATION_H
