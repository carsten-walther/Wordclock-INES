# Wordclock INES

**Another word clock, model INES**

[![Issues](https://img.shields.io/github/issues/carsten-walther/Wordclock-INES)](https://img.shields.io/github/issues/carsten-walther/Wordclock-INES)
[![Forks](https://img.shields.io/github/forks/carsten-walther/Wordclock-INES)](https://github.com/carsten-walther/Wordclock-INES/network/members)
[![Stars](https://img.shields.io/github/stars/carsten-walther/Wordclock-INES)](https://github.com/carsten-walther/Wordclock-INES/stargazers)
[![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/carsten-walther/Wordclock-INES)](https://github.com/carsten-walther/Wordclock-INES/releases/latest)
[![License](https://img.shields.io/github/license/carsten-walther/Wordclock-INES)](LICENSE.txt)
[![GitHub All Releases](https://img.shields.io/github/downloads/carsten-walther/Wordclock-INES/total)](https://github.com/carsten-walther/Wordclock-INES/releases/latest)

This clock shows the time in words.

![Wordclock INES Plate](Documentation/img/Plate-INES-de.png)

A detailed [build](Documentation/BUILD.md) and [operation](Documentation/README.md) manual can be found in [Documentation](Documentation) folder.

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

![GitHub followers](https://img.shields.io/github/followers/carsten-walther?style=social)
![GitHub forks](https://img.shields.io/github/forks/carsten-walther/Wordclock-INES?style=social)
![GitHub stars](https://img.shields.io/github/stars/carsten-walther/Wordclock-INES?style=social)
![GitHub watchers](https://img.shields.io/github/watchers/carsten-walther/Wordclock-INES?style=social)

[![Web](https://img.shields.io/badge/carstenwalther.de-blue.svg?logo=rss&style=social)](https://www.carstenwalther.de)
[![Facebook](https://img.shields.io/badge/carsten.walther-blue.svg?logo=facebook&style=social)](https://www.facebook.com/carsten.walther)
