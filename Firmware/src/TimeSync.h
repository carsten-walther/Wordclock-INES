//
// Created by Carsten Walther.
//

#ifndef WORDCLOCK_TIMESYNC_H
#define WORDCLOCK_TIMESYNC_H

#include <Arduino.h>

class TimeSync
{
public:
    void begin();
    void begin(const char *server1);
    void begin(const char *tz, const char *server1, const char *server2 = nullptr, const char *server3 = nullptr);
    static void config(int timezone_sec, int daylightOffset_sec);
    bool isSynced() const;
    int waitForSyncResult(unsigned long timeoutLength = 10000) const;
    static unsigned long getTimestamp();
    static int getHours();
    static int getMinutes();
    static int getSeconds();
    static int getSecondsSinceMidnight();

private:
    bool synced = false;

    void setTime(const char *tz, const char *server1, const char *server2, const char *server3);
};

extern TimeSync timeSync;

#endif // WORDCLOCK_TIMESYNC_H
