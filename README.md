# Wordclock INES

**Another word clock, model INES**

[![GitHub version](https://img.shields.io/github/release/carsten-walther/clock-INES.svg)](https://github.com/carsten-walther/clock-INES/releases/latest)
[![GitHub download](https://img.shields.io/github/downloads/carsten-walther/clock-INES/total.svg)](https://github.com/carsten-walther/clock-INES/releases/latest)
[![License](https://img.shields.io/github/license/carsten-walther/clock-INES.svg)](LICENSE.txt)


This clock shows the time in words.

## Requirements

The following is necessary to build this clock:

| amount | item |
|-:|:-|
| 1 | ESP8266 module |
| 25 | WS2812 RGB LED Strip |
| 1 | voltage regulator 3,3V |
| 1 | voltage regulator 5,0V |
| 1 | power supply |
| 1 | image frame 20x20cm (e.g. IKEA Ribba) |

## Libraries

included:
* Ticker v1.0

not included:
* [Adafruit NeoPixel v1.3.5](https://github.com/adafruit/Adafruit_NeoPixel.git)
* [NTPClient v3.1.0](https://github.com/arduino-libraries/NTPClient.git)
* [WifiManager v0.15.0](https://github.com/tzapu/WiFiManager.git)

## Languages

At the moment the front is only available in German (High German, Swabian, Bavarian, Saxon). The display of the language can be changed via the web interface.

## Operation

Various settings can be made via a web interface.

To make settings, connect to the clock via **[ines.local](http://ines.local)**.

### Colors / brightness

The color of the active (foreground color) and inactive (background color) words can be set via the Web UI using a color picker. Thus, all additive colors are possible. In addition, a slider can be used to define the brightness from 0% to 100%.

An automatic brightness adjustment is currently not implemented.

### Modes

Via a group switch the following modes are available

- Normal (time display)
- Scanner (Knight Rider effect) or
- Test (all LEDs light up in sequence)

### Settings

#### Switch on/off time
The switch-on or switch-off time can be used to specify the time from which the clock switches to sleep mode and wakes up again.

This deactivates the display of the time during this period and the clock thus saves power.

#### Time zone/ summer time control

The clock obtains the correct time from the Network Time Protocol (NTP) via an Internet connection. To ensure that the correct time is displayed, the time zone corresponding to the location can be selected. In addition, the daylight saving time control can be switched on or off via a switch.

#### Language

The desired language can be set via a selection menu. Thus the time can be displayed, for example, in High German "VIERTEL VOR ZWEI" or Saxon "DREIVIERTEL ZWEI".

## Support me / Follow me

[![Web](https://img.shields.io/badge/www-carstenwalther.de-blue.svg?style=flat-square&logo=www&colorB=3d72a8&colorA=333333)](https://www.carstenwalther.de)
[![Facebook](https://img.shields.io/badge/-%40carsten.walther-blue.svg?style=flat-square&logo=facebook&colorB=3d72a8&colorA=333333)](https://www.facebook.com/carsten.walther)
