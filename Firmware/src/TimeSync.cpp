#include "TimeSync.h"

#include <ctime>

TimeSync timeSync;

void TimeSync::begin()
{
    setTime(PSTR("UTC0"), "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org");
}

void TimeSync::begin(const char *server1)
{
    setTime(PSTR("UTC0"), server1, "1.pool.ntp.org", "2.pool.ntp.org");
}

void TimeSync::begin(const char *tz, const char *server1, const char *server2, const char *server3)
{
    setTime(tz, server1, server2, server3);
}

void TimeSync::config(int timezone_sec, int daylightOffset_sec)
{
    configTime(timezone_sec, daylightOffset_sec, "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org");
}

bool TimeSync::isSynced() const
{
    return synced;
}

int TimeSync::waitForSyncResult(unsigned long timeoutLength) const
{
    if (synced)
    {
        return 0;
    }

    unsigned long previousMillis = millis();
    unsigned long lastMillis = millis();

    while ((lastMillis - previousMillis) < timeoutLength)
    {
        yield();
        if (synced)
        {
            return 0;
        }
        lastMillis = millis();
    }

    return -1;
}

void TimeSync::setTime(const char *tz, const char *server1, const char *server2, const char *server3)
{
    configTzTime(tz, server1, server2, server3);
    synced = true;
}

unsigned long TimeSync::getTimestamp()
{
    time_t now = time(nullptr);
    return (int)now;
}

int TimeSync::getHours()
{
    time_t rawtime = time(nullptr);
    struct tm *ptm = localtime(&rawtime);
    return (int)ptm->tm_hour;
}

int TimeSync::getMinutes()
{
    time_t rawtime = time(nullptr);
    struct tm *ptm = localtime(&rawtime);
    return (int)ptm->tm_min;
}

int TimeSync::getSeconds()
{
    time_t rawtime = time(nullptr);
    struct tm *ptm = localtime(&rawtime);
    return (int)ptm->tm_sec;
}

int TimeSync::getSecondsSinceMidnight()
{
    time_t rawtime = time(nullptr);
    struct tm *ptm = localtime(&rawtime);
    return ((int)ptm->tm_hour * 60 * 60) + ((int)ptm->tm_min * 60) + (int)ptm->tm_sec;
}
