# 1 "/var/folders/3v/f4hyz79x6j7c8cbpn1l0smjh0000gn/T/tmp0DTQcL"
#include <Arduino.h>
# 1 "/Users/carstenwalther/Desktop/Wortuhr/Firmware/src/Main.ino"
# 11 "/Users/carstenwalther/Desktop/Wortuhr/Firmware/src/Main.ino"
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <NTPClient.h>


#include "Config.h"
#include "Timezones.h"
#include "Settings/Settings.h"



WiFiManager wifiManager;


ESP8266WebServer webServer(SERVER_PORT);


WiFiUDP wifiUdp;


MDNSResponder mdnsResponder;




NTPClient timeClient = NTPClient(wifiUdp, NTP_HOST_NAME, NTP_TIME_OFFSET, NTP_UPDATE_INTERVAL);
# 51 "/Users/carstenwalther/Desktop/Wortuhr/Firmware/src/Main.ino"
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(WS2811_NUMBER, WS2811_DATA_PIN, NEO_GRB + NEO_KHZ800);


ParametersType defaults = {

    255,
    255,
    255,

    0,
    0,
    0,

    33,
    0,
    false,
    1,
    0,
    5,
    0,
    0,
    SETTING_VERSION
};


Settings settings(defaults);


typedef struct {


    uint8_t R;
    uint8_t G;
    uint8_t B;
} RGB;


int clockMode = CLOCKMODE_NORMAL;
int clockModeOverride = -1;


unsigned long previousMillis_webServer_ticker = 0;
unsigned long previousMillis_clockMode_ticker = 0;
unsigned long previousMillis_debugging_ticker = 0;

unsigned long previousMillis_face = 0;
void setup();
void loop();
void clockMode_ticker();
void webServer_ticker();
void debugging_ticker();
void faceScanner();
void faceTest();
void faceNormal(uint16_t hours, uint16_t minutes);
void faceNight();
RGB getRGBFromHex(String hex);
uint8_t hexToInt(char upper, char lower);
String split(String data, char separator, int index);
void processTimeOffset();
void handleNotFound();
bool handleFileRead(String path);
void handleSettingsJson();
void handleUpdateJson();
String generateSettingsData();
String getContentType(String filename);
#line 100 "/Users/carstenwalther/Desktop/Wortuhr/Firmware/src/Main.ino"
void setup()
{
    #ifdef DEBUG
    Serial.begin(BAUD);
    Serial.printf("\n");
    Serial.printf("\n");
    Serial.printf("Starting Wordclock...\n");
    #endif


    pinMode(WS2811_DATA_PIN, OUTPUT);


    ledStrip.begin();
    ledStrip.setBrightness(0);
    ledStrip.clear();
    ledStrip.show();


    wifi_station_set_hostname(SERVER_HOST);

    wifiManager.setDebugOutput(false);




    if (!wifiManager.autoConnect(AP_SSID, AP_PASS)) {


        ESP.reset();
        delay(1000);

    } else {


        webServer.begin();


        webServer.onNotFound(handleNotFound);

        webServer.on("/settings.json", HTTP_GET, handleSettingsJson);

        webServer.on("/update.json", HTTP_POST, handleUpdateJson);


        if (mdnsResponder.begin(SERVER_HOST)) {
            mdnsResponder.addService("http", "tcp", SERVER_PORT);
        }


        fs::SPIFFSConfig cfg;
        cfg.setAutoFormat(false);
        SPIFFS.setConfig(cfg);
        SPIFFS.begin();
    }

    #ifdef DEBUG
    Serial.println("EEPROM:");
    Serial.println(generateSettingsData());
    #endif


    timeClient.begin();


    processTimeOffset();

    #ifdef DEBUG
    Serial.println("setup complete");
    #endif
}



void loop()
{
    clockMode_ticker();

    webServer_ticker();

    #ifdef DEBUG
    debugging_ticker();
    #endif


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



void clockMode_ticker()
{
    unsigned int speedDelay = 125;

    if (millis() - previousMillis_clockMode_ticker >= speedDelay) {

        previousMillis_clockMode_ticker = millis();

        clockMode = CLOCKMODE_NORMAL;


        if ((timeClient.getHours() == settings.parameters->sleepHour && timeClient.getMinutes() >= settings.parameters->sleepMinute) || (timeClient.getHours() == settings.parameters->wakeupHour && timeClient.getMinutes() < settings.parameters->wakeupMinute)) {
            clockMode = CLOCKMODE_NIGHT;
        }


        if (settings.parameters->sleepHour < settings.parameters->wakeupHour && timeClient.getHours() >= settings.parameters->sleepHour && timeClient.getHours() <= settings.parameters->wakeupHour) {
            clockMode = CLOCKMODE_NIGHT;
        }


        if (settings.parameters->sleepHour > settings.parameters->wakeupHour && (timeClient.getHours() >= settings.parameters->sleepHour || timeClient.getHours() <= settings.parameters->wakeupHour)) {
            clockMode = CLOCKMODE_NIGHT;
        }


        if (clockModeOverride > -1) {
            clockMode = clockModeOverride;
        }
    }
}

void webServer_ticker()
{
    unsigned int speedDelay = 50;

    if (millis() - previousMillis_webServer_ticker >= speedDelay) {

        previousMillis_webServer_ticker = millis();


        timeClient.update();


        webServer.handleClient();


        mdnsResponder.update();
    }
}

void debugging_ticker()
{
    unsigned int speedDelay = 1000;

    if (millis() - previousMillis_debugging_ticker >= speedDelay) {

        previousMillis_debugging_ticker = millis();

        Serial.printf("\n");
        Serial.printf("mode:\t\t\t%i\n", clockMode);
        Serial.printf("time:\t\t\t%i:%i\n", timeClient.getHours(), timeClient.getMinutes());
        Serial.printf("brightness:\t\t%i\n", settings.parameters->brightness);
        Serial.printf("timeZone:\t\t%i: %f\n", settings.parameters->timeZone, TIMEZONES[settings.parameters->timeZone]);
        Serial.printf("daylightSavingsTime:\t%i\n", settings.parameters->daylightSavingsTime);
        Serial.printf("foreground color:\t%i, %i, %i\n", settings.parameters->foregroundColorRed, settings.parameters->foregroundColorGreen, settings.parameters->foregroundColorBlue);
        Serial.printf("background color:\t%i, %i, %i\n", settings.parameters->backgroundColorRed, settings.parameters->backgroundColorGreen, settings.parameters->backgroundColorBlue);
    }
}



void faceScanner()
{
    unsigned int speedDelay = 125;

    if (millis() - previousMillis_face >= speedDelay) {

        previousMillis_face = millis();


        ledStrip.setBrightness(settings.parameters->brightness);


        for (int i = 12; i >= 9; i--) {

            for (int i = 0; i < ledStrip.numPixels(); i++) {
                ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
            }

            ledStrip.setPixelColor(i, ledStrip.Color(255, 0, 0));
            ledStrip.show();

            delay(speedDelay);
        }
        delay(speedDelay / 2);


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
    unsigned int speedDelay = 125;

    if (millis() - previousMillis_face >= speedDelay) {

        previousMillis_face = millis();


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
    unsigned int speedDelay = 1000;

    if (millis() - previousMillis_face >= speedDelay) {

        previousMillis_face = millis();

        while (hours < 0) {
            hours += 12;
        }

        while (hours > 24) {
            hours -= 12;
        }


        uint32_t foregroundCol = ledStrip.Color(settings.parameters->foregroundColorRed, settings.parameters->foregroundColorGreen, settings.parameters->foregroundColorBlue);
        uint32_t backgroundCol = ledStrip.Color(settings.parameters->backgroundColorRed, settings.parameters->backgroundColorGreen, settings.parameters->backgroundColorBlue);


        ledStrip.setBrightness(settings.parameters->brightness);

        for (int i = 0; i < ledStrip.numPixels(); i++) {
            ledStrip.setPixelColor(i, backgroundCol);
        }


        switch (minutes / 5) {
            case 0:

                ledStrip.setPixelColor(DE_GENAU, foregroundCol);
                break;
            case 1:

                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_NACH, foregroundCol);
                break;
            case 2:

                ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                ledStrip.setPixelColor(DE_NACH, foregroundCol);
                break;
            case 3:
                if ((settings.parameters->language == LANGUAGE_DE_SW) || (settings.parameters->language == LANGUAGE_DE_SA)) {

                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    hours = hours + 1;
                } else {

                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    ledStrip.setPixelColor(DE_NACH, foregroundCol);
                }
                break;
            case 4:
                if (settings.parameters->language == LANGUAGE_DE_SA) {

                    ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                    ledStrip.setPixelColor(DE_VOR, foregroundCol);
                    ledStrip.setPixelColor(DE_HALB, foregroundCol);
                    hours = hours + 1;
                } else {

                    ledStrip.setPixelColor(DE_ZWANZIG, foregroundCol);
                    ledStrip.setPixelColor(DE_NACH, foregroundCol);
                }
                break;
            case 5:

                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_VOR, foregroundCol);
                ledStrip.setPixelColor(DE_HALB, foregroundCol);
                hours = hours + 1;
                break;
            case 6:

                ledStrip.setPixelColor(DE_GENAU, foregroundCol);
                ledStrip.setPixelColor(DE_HALB, foregroundCol);
                hours = hours + 1;
                break;
            case 7:

                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_NACH, foregroundCol);
                ledStrip.setPixelColor(DE_HALB, foregroundCol);
                hours = hours + 1;
                break;
            case 8:
                if (settings.parameters->language == LANGUAGE_DE_SA) {

                    ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                    ledStrip.setPixelColor(DE_NACH, foregroundCol);
                    ledStrip.setPixelColor(DE_HALB, foregroundCol);
                    hours = hours + 1;
                } else {

                    ledStrip.setPixelColor(DE_ZWANZIG, foregroundCol);
                    ledStrip.setPixelColor(DE_VOR, foregroundCol);
                }
                break;
            case 9:
                if ((settings.parameters->language == LANGUAGE_DE_SW) || (settings.parameters->language == LANGUAGE_DE_BA) || (settings.parameters->language == LANGUAGE_DE_SA)) {

                    ledStrip.setPixelColor(DE_DREI, foregroundCol);
                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    hours = hours + 1;
                } else {

                    ledStrip.setPixelColor(DE_VIERTEL, foregroundCol);
                    ledStrip.setPixelColor(DE_VOR, foregroundCol);
                    hours = hours + 1;
                }
                break;
            case 10:

                ledStrip.setPixelColor(DE_ZEHN, foregroundCol);
                ledStrip.setPixelColor(DE_VOR, foregroundCol);
                hours = hours + 1;
                break;
            case 11:

                ledStrip.setPixelColor(DE_FUENF, foregroundCol);
                ledStrip.setPixelColor(DE_VOR, foregroundCol);
                hours = hours + 1;
                break;
        }


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
    unsigned int speedDelay = 1000;

    if (millis() - previousMillis_face >= speedDelay) {

        previousMillis_face = millis();

        ledStrip.setBrightness(0);
        ledStrip.clear();
        ledStrip.show();
    }
}



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


    timeClient.setTimeOffset(offset);

    timeClient.forceUpdate();
}



void handleNotFound()
{

    if (!handleFileRead(webServer.uri())) {
        webServer.send(404, "text/plain", "Error 404: File Not Found");
    }
}

bool handleFileRead(String path)
{

    if (path.endsWith("/")) {
        path += "index.html";
    }


    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";


    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {


        if (SPIFFS.exists(pathWithGz)) {

            path += ".gz";
        }


        File file = SPIFFS.open(path, "r");

        webServer.streamFile(file, contentType);

        file.close();

        return true;
    }


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

        RGB color;


        color = getRGBFromHex(webServer.arg("foregroundColor"));
        settings.parameters->foregroundColorRed = color.R;
        settings.parameters->foregroundColorGreen = color.G;
        settings.parameters->foregroundColorBlue = color.B;


        color = getRGBFromHex(webServer.arg("backgroundColor"));
        settings.parameters->backgroundColorRed = color.R;
        settings.parameters->backgroundColorGreen = color.G;
        settings.parameters->backgroundColorBlue = color.B;


        settings.parameters->brightness = map(webServer.arg("brightness").toInt(), 0, 100, 0, 255);


        settings.parameters->timeZone = webServer.arg("timeZone").toInt();


        if (webServer.arg("daylightSavingsTime") == String("true"))
            settings.parameters->daylightSavingsTime = true;
        else
            settings.parameters->daylightSavingsTime = false;


        settings.parameters->sleepHour = split(webServer.arg("sleepTime"), ':', 0).toInt();
        settings.parameters->sleepMinute = split(webServer.arg("sleepTime"), ':', 1).toInt();


        settings.parameters->wakeupHour = split(webServer.arg("wakeupTime"), ':', 0).toInt();
        settings.parameters->wakeupMinute = split(webServer.arg("wakeupTime"), ':', 1).toInt();


        settings.parameters->language = webServer.arg("language").toInt();


        if (webServer.hasArg("clockMode")) {
            clockModeOverride = webServer.arg("clockMode").toInt();
        }
    }


    settings.save();


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