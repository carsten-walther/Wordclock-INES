/*
 * Main
 *
 * @file Main.ino
 * @package .
 *
 * @author Carsten Walther
 */

// global libs
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// local libs
#include "Config.h"
#include "Timezones.h"
#include "Color/RGB.h"
#include "Ticker/Ticker.h"

// WiFiManager
// Once its business is done, there is no need to keep it around
WiFiManager wifiManager;

// Create a web server on port 80
ESP8266WebServer webServer(SERVER_PORT);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP wifiUdp;

// create a MDNS Responder instance
MDNSResponder mdnsResponder;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient = NTPClient(wifiUdp, NTP_HOST_NAME, NTP_TIME_OFFSET, NTP_UPDATE_INTERVAL);

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
// NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
// NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
// NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
// NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(WS2811_NUMBER, WS2811_DATA_PIN, NEO_GRB + NEO_KHZ800);

// foregroundColor
RGB foregroundColor = RGB(255, 255, 255);
// backgroundColor
RGB backgroundColor = RGB(0, 0, 0);
// brightness
int brightness = 33;
// timeZone
int timeZone = 0;
// daylightSavingsTime
bool daylightSavingsTime = false;
// sleepHour
int sleepHour = 1;
// sleepMinute
int sleepMinute = 0;
// wakeupHour
int wakeupHour = 5;
// wakeupMinute
int wakeupMinute = 0;
// clockMode
int clockMode = CLOCKMODE_NORMAL;
int clockModeOverride = -1;
// language
int language = 0;

// Ticker for clockMode
Ticker clockModeTicker;

// timer
unsigned long previousMillis = 0;

// =============================================================================

void setup()
{
    #ifdef DEBUG
    Serial.begin(BAUD);
    Serial.println(" ");
    Serial.println(" ");
    Serial.println("Starting Wordclock INES...");
    #endif

    // init WS2811 PIN
    pinMode(WS2811_DATA_PIN, OUTPUT);

    // init ws2811 LED Strip
    ledStrip.begin();
    ledStrip.setBrightness(0);
    ledStrip.clear();
    ledStrip.show();

    // init eeprom and hardware
    initEEPROM();

    // set host name
    wifi_station_set_hostname(SERVER_HOST);
    // disable library debug output
    wifiManager.setDebugOutput(false);
    // fetches ssid and pass and tries to connect
    // if it does not connect it starts an access point with the specified name
    // here  "AutoConnectAP"
    // and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect(AP_SSID, AP_PASS)) {

        // reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(1000);

    } else {

        // start the web server
        webServer.begin();
        // if someone requests any other file or page,
        // go to function 'handleNotFound' and check if the file exists
        webServer.onNotFound(handleNotFound);
        // if settings requested return settings
        webServer.on("/settings.json", HTTP_GET, handleSettingsJson);
        // if update requested update settings and return result
        webServer.on("/update.json", HTTP_POST, handleUpdateJson);

        // start the multicast domain name server
        if (mdnsResponder.begin(SERVER_HOST)) {
            #ifdef DEBUG
            Serial.println("MDNS responder started");
            #endif
            mdnsResponder.addService("http", "tcp", SERVER_PORT);
        }

        // start the SPI Flash File System (SPIFFS)
        fs::SPIFFSConfig cfg;
        cfg.setAutoFormat(false);
        SPIFFS.setConfig(cfg);
        SPIFFS.begin();
    }

    // start the ntp time client
    timeClient.begin();

    // process time offsets
    processTimeOffset();

    // attach clockMode
    clockModeTicker.attach_ms(125, clockModeTick);

    #ifdef DEBUG
    Serial.println("ready");
    #endif
}

// =============================================================================

void loop()
{
    // update the ntp time client
    timeClient.update();

    // run the web server
    webServer.handleClient();

    // update mdnsResponder service
    mdnsResponder.update();

    // update face
    switch (clockMode) {

        case CLOCKMODE_NORMAL:
            faceNormal(timeClient.getHours(), timeClient.getMinutes());
            break;

        case CLOCKMODE_NIGHT:
            faceNight();
            break;

        case CLOCKMODE_SCANNER:
            faceScanner();
            break;

        case CLOCKMODE_TEST:
            faceTest();
            break;
    }
}

// =============================================================================


void clockModeTick()
{
    clockMode = CLOCKMODE_NORMAL;

    // if time >= sleepTime or time <= wakeupTime
    if ((timeClient.getHours() == sleepHour && timeClient.getMinutes() >= sleepMinute) || (timeClient.getHours() == wakeupHour && timeClient.getMinutes() < wakeupMinute)) {
        clockMode = CLOCKMODE_NIGHT;
    }

    // if sleepTime < wakeupTime and hour > sleepHour and hour < wakeupHour
    if (sleepHour < wakeupHour && timeClient.getHours() >= sleepHour && timeClient.getHours() <= wakeupHour) {
        clockMode = CLOCKMODE_NIGHT;
    }

    // if sleepTime > wakeupTime and hour > sleepHour or hour < wakeupHour
    if (sleepHour > wakeupHour && (timeClient.getHours() >= sleepHour || timeClient.getHours() <= wakeupHour)) {
        clockMode = CLOCKMODE_NIGHT;
    }

    // override clockMode
    if (clockModeOverride > -1) {
        clockMode = clockModeOverride;
    }
}

// =============================================================================

void faceScanner()
{
    unsigned int speedDelay = 125;

    // 9, 10, 11, 12

    if (millis() - previousMillis >= speedDelay) {

        previousMillis = millis();

        // set brightness
        ledStrip.setBrightness(brightness);

        // RightToLeft
        for (int i = 12; i >= 9; i--) {

            for (int i = 0; i < ledStrip.numPixels(); i++) {
                ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
            }

            ledStrip.setPixelColor(i, ledStrip.Color(255, 0, 0));
            ledStrip.show();

            delay(speedDelay);
        }
        delay(speedDelay / 2);

        // LeftToRight
        for (int i = 9; i <= 12; i++) {

            for (int i = 0; i < ledStrip.numPixels(); i++) {
                ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
            }

            ledStrip.setPixelColor(i, ledStrip.Color(255, 0, 0));
            ledStrip.show();

            delay(speedDelay);
        }
        delay(speedDelay / 2);
    }
}

void faceTest()
{
    // set brightness
    ledStrip.setBrightness(brightness);

    for (int i = 0; i < ledStrip.numPixels(); i++) {
        ledStrip.setPixelColor(i, ledStrip.Color(255, 255, 255));
        delay(250);
        ledStrip.show();
    }

    for (int i = 0; i < ledStrip.numPixels(); i++) {
        ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
        delay(250);
        ledStrip.show();
    }
}

void faceNormal(uint16_t hours, uint16_t minutes)
{
    unsigned int speedDelay = 1000;

    if (millis() - previousMillis >= speedDelay) {

        previousMillis = millis();

        while (hours < 0) {
            hours += 12;
        }

        while (hours > 24) {
            hours -= 12;
        }

        #ifdef DEBUG
        Serial.printf("hours:\t\t%i\n", hours);
        Serial.printf("minutes:\t%i\n", minutes);
        Serial.printf("brightness:\t%i\n", brightness);
        #endif

        // set colors
        uint32_t foregroundCol = ledStrip.Color(foregroundColor.R, foregroundColor.G, foregroundColor.B);
        uint32_t backgroundCol = ledStrip.Color(backgroundColor.R, backgroundColor.G, backgroundColor.B);

        // set brightness
        ledStrip.setBrightness(brightness);

        for (int i = 0; i < ledStrip.numPixels(); i++) {
            ledStrip.setPixelColor(i, backgroundCol);
        }

        // minutes
        switch (minutes / 5) {
            case 0:
                // glatte Stunde
                ledStrip.setPixelColor(DE_GENAU, foregroundCol);
                break;
            case 1:
                // 5 nach
                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_NACH, foregroundCol);
                break;
            case 2:
                // 10 nach
                ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                ledStrip.setPixelColor(DE_NACH, foregroundCol);
                break;
            case 3:
                if ((language == LANGUAGE_DE_SW) || (language == LANGUAGE_DE_SA)) {
                    // viertel
                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    hours = hours + 1;
                } else {
                    // viertel nach
                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    ledStrip.setPixelColor(DE_NACH, foregroundCol);
                }
                break;
            case 4:
                if (language == LANGUAGE_DE_SA) {
                    // 10 vor halb
                    ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                    ledStrip.setPixelColor(DE_VOR, foregroundCol);
                    ledStrip.setPixelColor(DE_HALB, foregroundCol);
                    hours = hours + 1;
                } else {
                    // 20 nach
                    ledStrip.setPixelColor(DE_ZWANZIG, foregroundCol);
                    ledStrip.setPixelColor(DE_NACH, foregroundCol);
                }
                break;
            case 5:
                // 5 vor halb
                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_VOR, foregroundCol);
                ledStrip.setPixelColor(DE_HALB, foregroundCol);
                hours = hours + 1;
                break;
            case 6:
                // halb
                ledStrip.setPixelColor(DE_GENAU, foregroundCol);
                ledStrip.setPixelColor(DE_HALB, foregroundCol);
                hours = hours + 1;
                break;
            case 7:
                // 5 nach halb
                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_NACH, foregroundCol);
                ledStrip.setPixelColor(DE_HALB, foregroundCol);
                hours = hours + 1;
                break;
            case 8:
                if (language == LANGUAGE_DE_SA) {
                    // 10 nach halb
                    ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                    ledStrip.setPixelColor(DE_NACH, foregroundCol);
                    ledStrip.setPixelColor(DE_HALB, foregroundCol);
                    hours = hours + 1;
                } else {
                    // 20 vor
                    ledStrip.setPixelColor(DE_ZWANZIG, foregroundCol);
                    ledStrip.setPixelColor(DE_VOR, foregroundCol);
                }
                break;
            case 9:
                if ((language == LANGUAGE_DE_SW) || (language == LANGUAGE_DE_BA) || (language == LANGUAGE_DE_SA)) {
                    // drei viertel
                    ledStrip.setPixelColor(DE_DREI, foregroundCol);
                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    hours = hours + 1;
                } else {
                    // viertel vor
                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    ledStrip.setPixelColor(DE_VOR, foregroundCol);
                    hours = hours + 1;
                }
                break;
            case 10:
                // 10 vor
                ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                ledStrip.setPixelColor(DE_VOR, foregroundCol);
                hours = hours + 1;
                break;
            case 11:
                // 5 vor
                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_VOR, foregroundCol);
                hours = hours + 1;
                break;
        }

        // hours
        switch (hours) {
            case 0:
            case 12:
            case 24:
                ledStrip.setPixelColor(DE_H_ZWOELF, foregroundCol);
                break;
            case 1:
            case 13:
                ledStrip.setPixelColor(DE_H_EINS, foregroundCol);
                break;
            case 2:
            case 14:
                ledStrip.setPixelColor(DE_H_ZWEI, foregroundCol);
                break;
            case 3:
            case 15:
                ledStrip.setPixelColor(DE_H_DREI, foregroundCol);
                break;
            case 4:
            case 16:
                ledStrip.setPixelColor(DE_H_VIER, foregroundCol);
                break;
            case 5:
            case 17:
                ledStrip.setPixelColor(DE_H_FUENF, foregroundCol);
                break;
            case 6:
            case 18:
                ledStrip.setPixelColor(DE_H_SECHS, foregroundCol);
                break;
            case 7:
            case 19:
                ledStrip.setPixelColor(DE_H_SIEBEN, foregroundCol);
                break;
            case 8:
            case 20:
                ledStrip.setPixelColor(DE_H_ACHT, foregroundCol);
                break;
            case 9:
            case 21:
                ledStrip.setPixelColor(DE_H_NEUN, foregroundCol);
                break;
            case 10:
            case 22:
                ledStrip.setPixelColor(DE_H_ZEHN, foregroundCol);
                break;
            case 11:
            case 23:
                ledStrip.setPixelColor(DE_H_ELF, foregroundCol);
                break;
        }

        // minutes indicators
        switch (minutes % 5) {
            case 0:
                break;
            case 1:
                ledStrip.setPixelColor(DE_M_EINS, foregroundCol);
                break;
            case 2:
                ledStrip.setPixelColor(DE_M_EINS, foregroundCol);
                ledStrip.setPixelColor(DE_M_ZWEI, foregroundCol);
                break;
            case 3:
                ledStrip.setPixelColor(DE_M_EINS, foregroundCol);
                ledStrip.setPixelColor(DE_M_ZWEI, foregroundCol);
                ledStrip.setPixelColor(DE_M_DREI, foregroundCol);
                break;
            case 4:
                ledStrip.setPixelColor(DE_M_EINS, foregroundCol);
                ledStrip.setPixelColor(DE_M_ZWEI, foregroundCol);
                ledStrip.setPixelColor(DE_M_DREI, foregroundCol);
                ledStrip.setPixelColor(DE_M_VIER, foregroundCol);
                break;
        }

        ledStrip.show();
    }
}

void faceNight()
{
    ledStrip.setBrightness(0);

    for (int i = 0; i < ledStrip.numPixels(); i++) {
        ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
    }

    ledStrip.show();
}

// =============================================================================

RGB getRGBFromHex(String hex)
{
    hex.toUpperCase();
    char c[7];
    hex.toCharArray(c, 8);
    return RGB(hexToInt(c[1], c[2]), hexToInt(c[3], c[4]), hexToInt(c[5], c[6]));
}

int hexToInt(char upper, char lower)
{
    int uVal = (int)upper;
    int lVal = (int)lower;
    uVal = uVal > 64 ? uVal - 55 : uVal - 48;
    uVal = uVal << 4;
    lVal = lVal > 64 ? lVal - 55 : lVal - 48;
    return uVal + lVal;
}

String split(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length();

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void processTimeOffset()
{
    int offset = 0;

    if (daylightSavingsTime) {
        offset = offset + 3600;
    }

    offset = offset + TIMEZONES[timeZone] * 3600;

    // set time offset
    timeClient.setTimeOffset(offset);
    // uptade time
    timeClient.forceUpdate();
}

// =============================================================================

void initEEPROM()
{
    EEPROM.begin(512);

    //write a magic byte to eeprom 196 to determine if we've ever booted on this device before
    if (EEPROM.read(EEPROM_init) != 196) {
        //if not load default config files to EEPROM
        saveEEPROM();
    }

    // load config from EEPROM
    loadEEPROM();
}

void loadEEPROM()
{
    // EEPROM foregroundColor
    foregroundColor = RGB(EEPROM.read(EEPROM_foreground_R), EEPROM.read(EEPROM_foreground_G), EEPROM.read(EEPROM_foreground_B));
    // EEPROM backgroundColor
    backgroundColor = RGB(EEPROM.read(EEPROM_background_R), EEPROM.read(EEPROM_background_G), EEPROM.read(EEPROM_background_B));
    // EEPROM brightness
    brightness = EEPROM.read(EEPROM_brightness);
    // EEPROM timeZone
    timeZone = EEPROM.read(EEPROM_timeZone);
    // daylightSavingsTime
    daylightSavingsTime = EEPROM.read(EEPROM_daylightSavingsTime);
    // EEPROM sleepHour
    sleepHour = EEPROM.read(EEPROM_sleepHour);
    // EEPROM sleepMinute
    sleepMinute = EEPROM.read(EEPROM_sleepMinute);
    // EEPROM wakeupHour
    wakeupHour = EEPROM.read(EEPROM_wakeupHour);
    // EEPROM wakeupMinute
    wakeupMinute = EEPROM.read(EEPROM_wakeupMinute);
    // EEPROM language
    language = EEPROM.read(EEPROM_language);
}

void saveEEPROM()
{
    EEPROM.write(EEPROM_init, 196);
    // EEPROM foreground color
    EEPROM.write(EEPROM_foreground_R, foregroundColor.R);
    EEPROM.write(EEPROM_foreground_G, foregroundColor.G);
    EEPROM.write(EEPROM_foreground_B, foregroundColor.B);
    // EEPROM background color
    EEPROM.write(EEPROM_background_R, backgroundColor.R);
    EEPROM.write(EEPROM_background_G, backgroundColor.G);
    EEPROM.write(EEPROM_background_B, backgroundColor.B);
    // EEPROM brightness
    EEPROM.write(EEPROM_brightness, brightness);
    // EEPROM timeZone
    EEPROM.write(EEPROM_timeZone, timeZone);
    // daylightSavingsTime
    EEPROM.write(EEPROM_daylightSavingsTime, daylightSavingsTime);
    // EEPROM sleepHour
    EEPROM.write(EEPROM_sleepHour, sleepHour);
    // EEPROM sleepMinute
    EEPROM.write(EEPROM_sleepMinute, sleepMinute);
    // EEPROM wakeupHour
    EEPROM.write(EEPROM_wakeupHour, wakeupHour);
    // EEPROM wakeupMinute
    EEPROM.write(EEPROM_wakeupMinute, wakeupMinute);
    // EEPROM language
    EEPROM.write(EEPROM_language, language);

    EEPROM.commit();
}

void clearEEPROM()
{
    // write a 0 to all 512 bytes of the EEPROM
    for (int i = 0; i < 512; i++) {
        EEPROM.write(i, 0);
    }

    delay(500);

    EEPROM.commit();
    EEPROM.end();
}

// =============================================================================

void handleNotFound()
{
    // check if the file exists in the flash memory (SPIFFS), if so, send it
    if (!handleFileRead(webServer.uri())) {
        webServer.send(404, "text/plain", "Error 404: File Not Found");
    }
}

bool handleFileRead(String path)
{
    // If a folder is requested, send the index file
    if (path.endsWith("/")) {
        path += "index.html";
    }

    // Get the MIME type
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";

    // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {

        // If there's a compressed version available
        if (SPIFFS.exists(pathWithGz)) {
            // Use the compressed verion
            path += ".gz";
        }

        // Open the file
        File file = SPIFFS.open(path, "r");
        // Send it to the client
        webServer.streamFile(file, contentType);
        // Close the file again
        file.close();

        return true;
    }

    // If the file doesn't exist, return false
    return false;
}

void handleSettingsJson()
{
    String result = "";

    result = result + "{";
    result = result + "\"success\": true,";
    result = result + "\"result\": {";
    result = result + generateSettingsData();
    result = result + "}";
    result = result + "}";

    webServer.send(200, "application/json", result);
}

void handleUpdateJson()
{
    String result = "";

    bool success = true;

    if (
        !webServer.hasArg("foregroundColor") ||
        !webServer.hasArg("backgroundColor") ||
        !webServer.hasArg("brightness") ||
        !webServer.hasArg("timeZone") ||
        !webServer.hasArg("daylightSavingsTime") ||
        !webServer.hasArg("sleepTime") ||
        !webServer.hasArg("wakeupTime") ||
        !webServer.hasArg("language")
    ) {
        success = false;
    } else {

        // foregroundColor
        foregroundColor = getRGBFromHex(webServer.arg("foregroundColor"));

        // backgroundColor
        backgroundColor = getRGBFromHex(webServer.arg("backgroundColor"));

        // brightness
        brightness = map(webServer.arg("brightness").toInt(), 0, 100, 0, 255);

        // timeZone
        timeZone = webServer.arg("timeZone").toInt();

        // daylightSavingsTime
        if (webServer.arg("daylightSavingsTime") == String("true"))
            daylightSavingsTime = true;
        else
            daylightSavingsTime = false;

        // sleepTime (sleepHour, sleepMinute)
        sleepHour = split(webServer.arg("sleepTime"), ':', 0).toInt();
        sleepMinute = split(webServer.arg("sleepTime"), ':', 1).toInt();

        // wakeupTime (wakeupHour, wakeupMinute)
        wakeupHour = split(webServer.arg("wakeupTime"), ':', 0).toInt();
        wakeupMinute = split(webServer.arg("wakeupTime"), ':', 1).toInt();

        // clockModeOverride
        if (webServer.hasArg("clockMode")) {
            clockModeOverride = webServer.arg("clockMode").toInt();
        }

        // language
        language = webServer.arg("language").toInt();
    }

    // save settings
    saveEEPROM();
    // process time offsets
    processTimeOffset();

    result = result + "{";
    result = result + "\"success\": " + (success ? "true" : "false") + ",";
    result = result + "\"result\": {";
    result = result + generateSettingsData();
    result = result + "}";
    result = result + "}";

    webServer.send(200, "application/json", result);
}

String generateSettingsData()
{
    String result = "";

    result = result + "\"version\": \"" + VERSION + "\",";
    result = result + "\"clockMode\": " + clockMode + ",";
    result = result + "\"language\": " + language + ",";
    result = result + "\"foregroundColor\": {\"red\": " + foregroundColor.R + ", \"green\": " + foregroundColor.G + ", \"blue\": " + foregroundColor.B + "},";
    result = result + "\"backgroundColor\": {\"red\": " + backgroundColor.R + ", \"green\": " + backgroundColor.G + ", \"blue\": " + backgroundColor.B + "},";
    result = result + "\"brightness\": " + map(brightness, 0, 255, 0, 100) + ",";
    result = result + "\"timeZone\": " + timeZone + ",";
    result = result + "\"daylightSavingsTime\": " + (daylightSavingsTime ? "true" : "false") + ",";
    result = result + "\"sleepHour\": " + sleepHour + ",";
    result = result + "\"sleepMinute\": " + sleepMinute + ",";
    result = result + "\"wakeupHour\": " + wakeupHour + ",";
    result = result + "\"wakeupMinute\": " + wakeupMinute + ",";
    result = result + "\"time\": \"" + timeClient.getFormattedTime() + "\"";

    return result;
}

String getContentType(String filename)
{
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".gz"))
        return "application/x-gzip";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".svg"))
        return "image/svg+xml";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    return "text/plain";
}
