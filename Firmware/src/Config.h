
#ifndef CONFIG_H_
#define CONFIG_H_

//
// Version =====================================================================
//

#define VERSION "0.0.1"

//
// Debug/Serial Settings =======================================================
//

// Should the debugging function be activated via Serial?
//#define DEBUG

// Serial baud rate
#define BAUD 115200

//
// Access Point settings =======================================================
//

// Access Point Hostname (.local is attached to the domain automatically by mDNS)
#define AP_HOST "ines"

// Access Point SSID
#define AP_SSID "Wordclock Ines"

// Access Point Passwort
#define AP_PASS "password"

// Web Server Port
#define AP_PORT 80

//
// NTP settings ================================================================
//

// NTP Hostname
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
// WS2811 LED Strip ============================================================
//

// To which pin is the WS2811 LED Strip connected?
// Default: 2
#define WS2811_DATA_PIN 2

// How many LEDs are there?
// Default: 25
#define WS2811_NUMBER 25

//
// EEPROM Settings =============================================================
//

// At which addresses in the EEPROM should which settings be stored?
#define EEPROM_init 500
#define EEPROM_foreground_R 100
#define EEPROM_foreground_G 101
#define EEPROM_foreground_B 102
#define EEPROM_background_R 103
#define EEPROM_background_G 104
#define EEPROM_background_B 105
#define EEPROM_brightness 200
#define EEPROM_timeZone 201
#define EEPROM_daylightSavingsTime 202
#define EEPROM_sleepHour 203
#define EEPROM_sleepMinute 204
#define EEPROM_wakeupHour 205
#define EEPROM_wakeupMinute 206
#define EEPROM_language 207

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

#define DE_GENAU        0
#define DE_DREI         1
#define DE_VIERTEL      2
#define DE_ZEHN         3
#define DE_FUENF        4
#define DE_ZWANZIG      5
#define DE_NACH         6
#define DE_VOR          7
#define DE_HALB         8

#define DE_M_EINS      12
#define DE_M_ZWEI      11
#define DE_M_DREI      10
#define DE_M_VIER       9

#define DE_H_EINS      13
#define DE_H_ZWEI      14
#define DE_H_DREI      15
#define DE_H_VIER      16
#define DE_H_FUENF     17
#define DE_H_SECHS     18
#define DE_H_SIEBEN    19
#define DE_H_ACHT      20
#define DE_H_NEUN      21
#define DE_H_ZEHN      22
#define DE_H_ELF       23
#define DE_H_ZWOELF    24

// Language

#define LANGUAGE_DE_DE  0 // Hochdeutsch
#define LANGUAGE_DE_SW  1 // Schwaebisch
#define LANGUAGE_DE_BA  2 // Bayrisch
#define LANGUAGE_DE_SA  3 // Saechsisch

//
// Certs =======================================================================
//



#endif // CONFIG_H_
