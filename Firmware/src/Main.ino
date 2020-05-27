/*
 * Main
 *
 * @file Main.ino
 * @package .
 *
 * @author Carsten Walther
 */

// global libs
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// local libs
#include "Config.h"
#include "Timezones.h"
#include "Settings/Settings.h"

// WiFiManager
// Once its business is done, there is no need to keep it around
WiFiManager wifiManager;

// Create a web server on port 80
ESP8266WebServer HTTP(SERVER_PORT);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP UDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient NTP = NTPClient(UDP, NTP_HOST_NAME, NTP_TIME_OFFSET, NTP_UPDATE_INTERVAL);

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
// NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
// NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
// NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
// NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(WS2811_NUMBER, WS2811_DATA_PIN, NEO_GRB + NEO_KHZ800);

// Parameters Structure
ParametersType defaults = {
    // foreground color
    255,            // foregroundColorRed
    255,            // foregroundColorGreen
    255,            // foregroundColorBlue
    // background color
    0,              // backgroundColorRed
    0,              // backgroundColorGreen
    0,              // backgroundColorBlue
    // settings
    33,             // brightness
    0,              // timeZone
    false,          // daylightSavingsTime
    1,              // sleepHour
    0,              // sleepMinute
    5,              // wakeupHour
    0,              // wakeupMinute
    0,              // language
    SETTING_VERSION // Version
};

// Settings
Settings settings(defaults);

// color RGB
typedef struct {
    // Red, Green, Blue color members (0-255) where
    // (0,0,0) is black and (255,255,255) is white
    uint8_t R;
    uint8_t G;
    uint8_t B;
} RGB;

// clockMode
int clockMode = CLOCKMODE_NORMAL;
int clockModeOverride = -1;

// current millis
unsigned long currentMillis = 0;
// general timer
unsigned long previousMillis_HTTP_ticker = 0;
unsigned long previousMillis_clockMode_ticker = 0;
unsigned long previousMillis_debugging_ticker = 0;
// face timer
unsigned long previousMillis_face = 0;

// =============================================================================

void setup()
{
    #ifdef DEBUG
    Serial.begin(BAUD);
    Serial.printf("\n");
    Serial.printf("\n");
    Serial.printf("Starting Wordclock...\n");
    #endif

    // init WS2811 PIN
    pinMode(WS2811_DATA_PIN, OUTPUT);

    // init ws2811 LED Strip
    ledStrip.begin();
    ledStrip.setBrightness(0);
    ledStrip.clear();
    ledStrip.show();

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

        // if someone requests any other file or page,
        // go to function 'handleNotFound' and check if the file exists
        HTTP.onNotFound(handleNotFound);
        // if settings requested return settings
        HTTP.on("/settings.json", HTTP_GET, handleSettingsJson);
        // if update requested update settings and return result
        HTTP.on("/update.json", HTTP_POST, handleUpdateJson);
        // if ssdp return description
        HTTP.on("/description.xml", HTTP_GET, handleSSDP);

        // start the web server
        HTTP.begin();
        #ifdef DEBUG
        Serial.println("HTTP started");
        #endif

        // start the multicast domain name server
        if (MDNS.begin(SERVER_HOST)) {
            MDNS.addService("http", "tcp", SERVER_PORT);
            #ifdef DEBUG
            Serial.println("MDNS started");
            #endif
        }

        // start the SPI Flash File System (LittleFS)
        if (LittleFS.begin()) {
            #ifdef DEBUG
            Serial.println("LittleFS started");
            #endif
        }

        // init SSDP
        SSDP.setSchemaURL("description.xml");
        SSDP.setHTTPPort(SERVER_PORT);
        SSDP.setName(AP_SSID);
        SSDP.setSerialNumber(ESP.getChipId());
        SSDP.setURL("/");
        SSDP.setModelName(AP_SSID);
        SSDP.setModelNumber(VERSION);
        SSDP.setModelURL("https://github.com/carsten-walther/wordclock");
        SSDP.setManufacturer("Carsten Walther");
        SSDP.setManufacturerURL("https://www.carstenwalther.de");
        SSDP.setDeviceType("upnp:rootdevice");
        if (SSDP.begin()) {
            #ifdef DEBUG
            Serial.println("SSDP started");
            #endif
        }
    }

    #ifdef DEBUG
    Serial.println("EEPROM:");
    Serial.println(generateSettingsData());
    #endif

    // start the ntp time client
    NTP.begin();

    // process time offsets
    processTimeOffset();

    #ifdef DEBUG
    Serial.println("setup complete");
    #endif
}

// =============================================================================

void loop()
{
    currentMillis = millis();

    clockMode_ticker();

    HTTP_ticker();

    #ifdef DEBUG
    debugging_ticker();
    #endif

    // update face
    switch (clockMode) {

        case CLOCKMODE_NORMAL:
            faceNormal(NTP.getHours(), NTP.getMinutes());
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

void clockMode_ticker()
{
    unsigned long speedDelay = 125;

    if (currentMillis - previousMillis_clockMode_ticker > speedDelay) {

        previousMillis_clockMode_ticker = currentMillis;

        clockMode = CLOCKMODE_NORMAL;

        // if time >= sleepTime or time <= wakeupTime
        if ((NTP.getHours() == settings.parameters->sleepHour && NTP.getMinutes() >= settings.parameters->sleepMinute) || (NTP.getHours() == settings.parameters->wakeupHour && NTP.getMinutes() < settings.parameters->wakeupMinute)) {
            clockMode = CLOCKMODE_NIGHT;
        }

        // if sleepTime < wakeupTime and hour > sleepHour and hour < wakeupHour
        if (settings.parameters->sleepHour < settings.parameters->wakeupHour && NTP.getHours() >= settings.parameters->sleepHour && NTP.getHours() <= settings.parameters->wakeupHour) {
            clockMode = CLOCKMODE_NIGHT;
        }

        // if sleepTime > wakeupTime and hour > sleepHour or hour < wakeupHour
        if (settings.parameters->sleepHour > settings.parameters->wakeupHour && (NTP.getHours() >= settings.parameters->sleepHour || NTP.getHours() <= settings.parameters->wakeupHour)) {
            clockMode = CLOCKMODE_NIGHT;
        }

        // override clockMode
        if (clockModeOverride > -1) {
            clockMode = clockModeOverride;
        }
    }
}

void HTTP_ticker()
{
    unsigned long speedDelay = 50;

    if (currentMillis - previousMillis_HTTP_ticker > speedDelay) {

        previousMillis_HTTP_ticker = currentMillis;

        // update the ntp time client
        NTP.update();

        // update MDNS service
        MDNS.update();

        // handle HTTP client
        HTTP.handleClient();
    }
}

void debugging_ticker()
{
    unsigned long speedDelay = 1000;

    if (currentMillis - previousMillis_debugging_ticker > speedDelay) {

        previousMillis_debugging_ticker = currentMillis;

        Serial.printf("\n");
        Serial.printf("mode:\t\t\t%i\n", clockMode);
        Serial.printf("time:\t\t\t%i:%i\n", NTP.getHours(), NTP.getMinutes());
        Serial.printf("brightness:\t\t%i\n", settings.parameters->brightness);
        Serial.printf("timeZone:\t\t%i: %f\n", settings.parameters->timeZone, TIMEZONES[settings.parameters->timeZone]);
        Serial.printf("daylightSavingsTime:\t%i\n", settings.parameters->daylightSavingsTime);
        Serial.printf("foreground color:\t%i, %i, %i\n", settings.parameters->foregroundColorRed, settings.parameters->foregroundColorGreen, settings.parameters->foregroundColorBlue);
        Serial.printf("background color:\t%i, %i, %i\n", settings.parameters->backgroundColorRed, settings.parameters->backgroundColorGreen, settings.parameters->backgroundColorBlue);
    }
}

// =============================================================================

void faceScanner()
{
    unsigned long speedDelay = 125;

    if (currentMillis - previousMillis_face > speedDelay) {

        previousMillis_face = currentMillis;

        // set brightness
        ledStrip.setBrightness(settings.parameters->brightness);

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
    unsigned long speedDelay = 125;

    if (currentMillis - previousMillis_face > speedDelay) {

        previousMillis_face = currentMillis;

        // set brightness
        ledStrip.setBrightness(settings.parameters->brightness);

        for (int i = 0; i < ledStrip.numPixels(); i++) {
            ledStrip.setPixelColor(i, ledStrip.Color(255, 255, 255));
            delay(speedDelay);
            ledStrip.show();
        }

        for (int i = 0; i < ledStrip.numPixels(); i++) {
            ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
            delay(speedDelay);
            ledStrip.show();
        }
    }
}

void faceNormal(uint16_t hours, uint16_t minutes)
{
    unsigned long speedDelay = 1000;

    if (currentMillis - previousMillis_face > speedDelay) {

        previousMillis_face = currentMillis;

        while (hours < 0) {
            hours += 12;
        }

        while (hours > 24) {
            hours -= 12;
        }

        // set colors
        uint32_t foregroundCol = ledStrip.Color(settings.parameters->foregroundColorRed, settings.parameters->foregroundColorGreen, settings.parameters->foregroundColorBlue);
        uint32_t backgroundCol = ledStrip.Color(settings.parameters->backgroundColorRed, settings.parameters->backgroundColorGreen, settings.parameters->backgroundColorBlue);

        // set brightness
        ledStrip.setBrightness(settings.parameters->brightness);

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
                if ((settings.parameters->language == LANGUAGE_DE_SW) || (settings.parameters->language == LANGUAGE_DE_SA)) {
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
                if (settings.parameters->language == LANGUAGE_DE_SA) {
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
                if (settings.parameters->language == LANGUAGE_DE_SA) {
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
                if ((settings.parameters->language == LANGUAGE_DE_SW) || (settings.parameters->language == LANGUAGE_DE_BA) || (settings.parameters->language == LANGUAGE_DE_SA)) {
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
    unsigned long speedDelay = 1000;

    if (currentMillis - previousMillis_face > speedDelay) {

        previousMillis_face = currentMillis;

        ledStrip.setBrightness(0);
        ledStrip.clear();
        ledStrip.show();
    }
}

// =============================================================================

RGB getRGBFromHex(String hex)
{
    hex.toUpperCase();
    char c[7];
    hex.toCharArray(c, 8);
    return {
        hexToInt(c[1], c[2]),
        hexToInt(c[3], c[4]),
        hexToInt(c[5], c[6])
    };
}

uint8_t hexToInt(char upper, char lower)
{
    uint8_t uVal = (uint8_t)upper;
    uint8_t lVal = (uint8_t)lower;
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

    if (settings.parameters->daylightSavingsTime) {
        offset = offset + 3600;
    }

    offset = offset + TIMEZONES[settings.parameters->timeZone] * 3600;

    // set time offset
    NTP.setTimeOffset(offset);
    // uptade time
    NTP.forceUpdate();
}


void delay(unsigned long ms)
{
    unsigned long start = millis();
    for (;;) {
        unsigned long now = millis();
        unsigned long elapsed = now - start;
        if (elapsed >= ms) {
            return;
        }
    }
}

// =============================================================================

void handleNotFound()
{
    // check if the file exists in the flash memory (LittleFS), if so, send it
    if (!handleFileRead(HTTP.uri())) {
        HTTP.send(404, "text/plain", "Error 404: File Not Found");
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
    if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {

        // If there's a compressed version available
        if (LittleFS.exists(pathWithGz)) {
            // Use the compressed verion
            path += ".gz";
        }

        // Open the file
        File file = LittleFS.open(path, "r");
        // Send it to the client
        HTTP.streamFile(file, contentType);
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

    HTTP.send(200, "application/json", result);
}

void handleUpdateJson()
{
    String result = "";

    bool success = true;

    if (
        !HTTP.hasArg("foregroundColor") ||
        !HTTP.hasArg("backgroundColor") ||
        !HTTP.hasArg("brightness") ||
        !HTTP.hasArg("timeZone") ||
        !HTTP.hasArg("daylightSavingsTime") ||
        !HTTP.hasArg("sleepTime") ||
        !HTTP.hasArg("wakeupTime") ||
        !HTTP.hasArg("language")
    ) {
        success = false;
    } else {

        RGB color;

        // foregroundColor
        color = getRGBFromHex(HTTP.arg("foregroundColor"));
        settings.parameters->foregroundColorRed = color.R;
        settings.parameters->foregroundColorGreen = color.G;
        settings.parameters->foregroundColorBlue = color.B;

        // backgroundColor
        color = getRGBFromHex(HTTP.arg("backgroundColor"));
        settings.parameters->backgroundColorRed = color.R;
        settings.parameters->backgroundColorGreen = color.G;
        settings.parameters->backgroundColorBlue = color.B;

        // brightness
        settings.parameters->brightness = map(HTTP.arg("brightness").toInt(), 0, 100, 0, 255);

        // timeZone
        settings.parameters->timeZone = HTTP.arg("timeZone").toInt();

        // daylightSavingsTime
        if (HTTP.arg("daylightSavingsTime") == String("true"))
            settings.parameters->daylightSavingsTime = true;
        else
            settings.parameters->daylightSavingsTime = false;

        // sleepTime (sleepHour, sleepMinute)
        settings.parameters->sleepHour = split(HTTP.arg("sleepTime"), ':', 0).toInt();
        settings.parameters->sleepMinute = split(HTTP.arg("sleepTime"), ':', 1).toInt();

        // wakeupTime (wakeupHour, wakeupMinute)
        settings.parameters->wakeupHour = split(HTTP.arg("wakeupTime"), ':', 0).toInt();
        settings.parameters->wakeupMinute = split(HTTP.arg("wakeupTime"), ':', 1).toInt();

        // language
        settings.parameters->language = HTTP.arg("language").toInt();

        // clockModeOverride
        if (HTTP.hasArg("clockMode")) {
            clockModeOverride = HTTP.arg("clockMode").toInt();
        }
    }

    // save settings
    settings.save();

    // process time offsets
    processTimeOffset();

    result = result + "{";
    result = result + "\"success\": " + (success ? "true" : "false") + ",";
    result = result + "\"result\": {";
    result = result + generateSettingsData();
    result = result + "}";
    result = result + "}";

    HTTP.send(200, "application/json", result);
}

void handleSSDP()
{
    SSDP.schema(HTTP.client());
}

String generateSettingsData()
{
    String result = "";

    result = result + "\"foregroundColor\": {\"red\": " + settings.parameters->foregroundColorRed + ", \"green\": " + settings.parameters->foregroundColorGreen + ", \"blue\": " + settings.parameters->foregroundColorBlue + "},";
    result = result + "\"backgroundColor\": {\"red\": " + settings.parameters->backgroundColorRed + ", \"green\": " + settings.parameters->backgroundColorGreen + ", \"blue\": " + settings.parameters->backgroundColorBlue + "},";
    result = result + "\"brightness\": " + map(settings.parameters->brightness, 0, 255, 0, 100) + ",";
    result = result + "\"timeZone\": " + settings.parameters->timeZone + ",";
    result = result + "\"daylightSavingsTime\": " + (settings.parameters->daylightSavingsTime ? "true" : "false") + ",";
    result = result + "\"sleepHour\": " + settings.parameters->sleepHour + ",";
    result = result + "\"sleepMinute\": " + settings.parameters->sleepMinute + ",";
    result = result + "\"wakeupHour\": " + settings.parameters->wakeupHour + ",";
    result = result + "\"wakeupMinute\": " + settings.parameters->wakeupMinute + ",";
    result = result + "\"language\": " + settings.parameters->language + ",";
    result = result + "\"clockMode\": " + clockMode + ",";
    result = result + "\"version\": \"" + VERSION + "\",";
    result = result + "\"eeprom\": \"" + SETTING_VERSION + "\",";
    result = result + "\"time\": \"" + NTP.getFormattedTime() + "\"";

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
