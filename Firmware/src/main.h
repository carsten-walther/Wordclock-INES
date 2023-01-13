//
// Created by Carsten Walther.
//

#ifndef WORDCLOCK_MAIN_H
#define WORDCLOCK_MAIN_H

#include <Arduino.h>

//
// Version =====================================================================
//

// Software Version
#define VERSION "2.0.0 beta"

//
// Debug/Serial Settings =======================================================
//

// Should the debugging function be activated via Serial?
// #define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

// Serial baud rate
#define BAUD 115200

//
// Select the FileSystem by uncommenting one of the lines below ================
//

#define USE_SPIFFS
//#define USE_LITTLEFS
//#define USE_SDFS

//
// Access Point settings =======================================================
//

// Access Point SSID char[32]
#define AP_SSID "WordClock"

//
// Webserver ===================================================================
//

// Access Point Hostname (.local is attached to the domain automatically by mDNS) char[32]
#define SERVER_HOST "wordclock"

// Web Server Ports
#define SERVER_PORT 80

//
// NTP settings ================================================================
//

// NTP Hostname char[64]
#define NTP_HOST_NAME "europe.pool.ntp.org"

//
// Time Settings ===============================================================
//

// After how many seconds should the clock be synchronized?
// Default: 60000
#define NTP_UPDATE_INTERVAL 60000

// Time offset?
// Default: 3600
#define NTP_TIME_OFFSET 3600

//
// OTA =========================================================================
//

//#define USE_OTA

#define OTA_PASS "password"

#define OTA_PORT 8266

//
// AUTH ========================================================================
//

#define USE_BASIC_AUTH true

#define AUTH_REALM "WordClock"

// char[32]
#define AUTH_USERNAME "admin"

// char[32]
#define AUTH_PASSWORD "password"

//
// WS2811 LED Strip ============================================================
//

// To which pin is the WS2811 LED Strip connected?
// Default: 2
#define WS2811_DATA_PIN 2

// How many LEDs are there?
// Default: 25
#define WS2811_NUMBER 25

//
// Clockmodes ==================================================================
//

// Which modes can be used?
#define CLOCKMODE_NIGHT 0
#define CLOCKMODE_NORMAL 1
#define CLOCKMODE_SCANNER 2
#define CLOCKMODE_TEST 3

//
// Words =======================================================================
//

#define DE_GENAU 0
#define DE_DREI 1
#define DE_VIERTEL 2
#define DE_ZEHN 3
#define DE_FUENF 4
#define DE_ZWANZIG 5
#define DE_NACH 6
#define DE_VOR 7
#define DE_HALB 8

#define DE_M_EINS 12
#define DE_M_ZWEI 11
#define DE_M_DREI 10
#define DE_M_VIER 9

#define DE_H_EINS 13
#define DE_H_ZWEI 14
#define DE_H_DREI 15
#define DE_H_VIER 16
#define DE_H_FUENF 17
#define DE_H_SECHS 18
#define DE_H_SIEBEN 19
#define DE_H_ACHT 20
#define DE_H_NEUN 21
#define DE_H_ZEHN 22
#define DE_H_ELF 23
#define DE_H_ZWOELF 24

// Language

#define LANGUAGE_DE_DE 0 // Hochdeutsch
#define LANGUAGE_DE_SW 1 // Schwaebisch
#define LANGUAGE_DE_BA 2 // Bayrisch
#define LANGUAGE_DE_SA 3 // Saechsisch

#endif // WORDCLOCK_MAIN_H
