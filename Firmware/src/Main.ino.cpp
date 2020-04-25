# 1 "/var/folders/3v/f4hyz79x6j7c8cbpn1l0smjh0000gn/T/tmpRgNztD"
#include <Arduino.h>
# 1 "/Users/carstenwalther/Desktop/Wortuhr INES/Firmware/src/Main.ino"

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
#include <Ticker.h>


#include "Config.h"
#include "Color/RGB.h"



WiFiManager wifiManager;


ESP8266WebServer webServer(AP_PORT);


WiFiUDP wifiUdp;


MDNSResponder mdnsResponder;




NTPClient timeClient = NTPClient(wifiUdp, NTP_HOST_NAME, NTP_TIME_OFFSET, NTP_UPDATE_INTERVAL);
# 44 "/Users/carstenwalther/Desktop/Wortuhr INES/Firmware/src/Main.ino"
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(WS2811_NUMBER, WS2811_DATA_PIN, NEO_GRB + NEO_KHZ800);


RGB foregroundColor = RGB(225, 0, 0);

RGB backgroundColor = RGB(0, 0, 255);

int brightness = 10;

int timeZone = 1;

bool daylightSavingsTime = true;

int sleepHour = 1;

int sleepMinute = 0;

int wakeupHour = 5;

int wakeupMinute = 0;

int clockMode = CLOCKMODE_NORMAL;


Ticker webServerTicker;

Ticker clockModeTicker;
void setup();
void loop();
void webServerTick();
void clockModeTick();
void faceNormal(uint16_t hours, uint16_t minutes);
void faceNight();
RGB getRGBFromHex(String hex);
int hexToInt(char upper, char lower);
String split(String data, char separator, int index);
void initEEPROM();
void loadEEPROM();
void saveEEPROM();
void clearEEPROM();
void handleNotFound();
bool handleFileRead(String path);
void handleSettingsJson();
void handleUpdateJson();
String getContentType(String filename);
#line 74 "/Users/carstenwalther/Desktop/Wortuhr INES/Firmware/src/Main.ino"
void setup()
{
    #ifdef DEBUG
    Serial.begin(BAUD);
    Serial.println(" ");
    Serial.println("Starting Wordclock INES...");
    Serial.flush();
    #endif


    pinMode(WS2811_DATA_PIN, OUTPUT);


    ledStrip.begin();
    ledStrip.setBrightness(0);
    ledStrip.clear();
    ledStrip.show();


    initEEPROM();


    wifi_station_set_hostname(AP_HOST);

    wifiManager.setDebugOutput(false);




    if (!wifiManager.autoConnect(AP_SSID, AP_PASS)) {


        ESP.reset();
        delay(1000);

    } else {


        webServer.begin();


        webServer.onNotFound(handleNotFound);

        webServer.on("/settings.json", HTTP_GET, handleSettingsJson);

        webServer.on("/update.json", HTTP_POST, handleUpdateJson);


        mdnsResponder.begin(AP_HOST);

        mdnsResponder.addService("http", "tcp", AP_PORT);


        fs::SPIFFSConfig cfg;
        cfg.setAutoFormat(false);
        SPIFFS.setConfig(cfg);
        SPIFFS.begin();


        timeClient.begin();

        #ifdef DEBUG
        Serial.println("ready");
        #endif
    }


    webServerTicker.attach_ms(250, webServerTick);

    clockModeTicker.attach_ms(1000, clockModeTick);
}



void loop()
{

    switch (clockMode) {

        case CLOCKMODE_NORMAL:
            faceNormal(timeClient.getHours(), timeClient.getMinutes());
            break;

        case CLOCKMODE_NIGHT:
            faceNight();
            break;
    }

    ledStrip.show();
}



void webServerTick()
{

    timeClient.update();

    webServer.handleClient();

    mdnsResponder.update();
}

void clockModeTick()
{
    clockMode = CLOCKMODE_NORMAL;


    if ((timeClient.getHours() == sleepHour && timeClient.getMinutes() >= sleepMinute) || (timeClient.getHours() == wakeupHour && timeClient.getMinutes() < wakeupMinute)) {
        clockMode = CLOCKMODE_NIGHT;
    }


    if (sleepHour < wakeupHour && timeClient.getHours() >= sleepHour && timeClient.getHours() <= wakeupHour) {
        clockMode = CLOCKMODE_NIGHT;
    }


    if (sleepHour > wakeupHour && (timeClient.getHours() >= sleepHour || timeClient.getHours() <= wakeupHour)) {
        clockMode = CLOCKMODE_NIGHT;
    }
}



void faceNormal(uint16_t hours, uint16_t minutes)
{

    ledStrip.setBrightness(brightness);
}

void faceNight()
{
    for (int i = 0; i < ledStrip.numPixels(); i++) {
        ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
    }

    ledStrip.setBrightness(brightness);
}



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



void initEEPROM()
{
    EEPROM.begin(512);


    if (EEPROM.read(EEPROM_init) != 196) {

        saveEEPROM();
    }


    loadEEPROM();
}

void loadEEPROM()
{

    foregroundColor = RGB(EEPROM.read(EEPROM_foreground_R), EEPROM.read(EEPROM_foreground_G), EEPROM.read(EEPROM_foreground_B));

    backgroundColor = RGB(EEPROM.read(EEPROM_background_R), EEPROM.read(EEPROM_background_G), EEPROM.read(EEPROM_background_B));

    brightness = EEPROM.read(EEPROM_brightness);

    timeZone = EEPROM.read(EEPROM_timeZone);

    daylightSavingsTime = EEPROM.read(EEPROM_daylightSavingsTime);

    sleepHour = EEPROM.read(EEPROM_sleepHour);

    sleepMinute = EEPROM.read(EEPROM_sleepMinute);

    wakeupHour = EEPROM.read(EEPROM_wakeupHour);

    wakeupMinute = EEPROM.read(EEPROM_wakeupMinute);
}

void saveEEPROM()
{
    EEPROM.write(EEPROM_init, 196);

    EEPROM.write(EEPROM_foreground_R, foregroundColor.R);
    EEPROM.write(EEPROM_foreground_G, foregroundColor.G);
    EEPROM.write(EEPROM_foreground_B, foregroundColor.B);

    EEPROM.write(EEPROM_background_R, backgroundColor.R);
    EEPROM.write(EEPROM_background_G, backgroundColor.G);
    EEPROM.write(EEPROM_background_B, backgroundColor.B);

    EEPROM.write(EEPROM_brightness, brightness);

    EEPROM.write(EEPROM_timeZone, timeZone);

    EEPROM.write(EEPROM_daylightSavingsTime, daylightSavingsTime);

    EEPROM.write(EEPROM_sleepHour, sleepHour);

    EEPROM.write(EEPROM_sleepMinute, sleepMinute);

    EEPROM.write(EEPROM_wakeupHour, wakeupHour);

    EEPROM.write(EEPROM_wakeupMinute, wakeupMinute);

    EEPROM.commit();
}

void clearEEPROM()
{

    for (int i = 0; i < 512; i++) {
        EEPROM.write(i, 0);
    }

    delay(500);

    EEPROM.commit();
    EEPROM.end();
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
    result = result + "\"version\": \"" + VERSION + "\",";
    result = result + "\"foregroundColor\": {\"red\": " + foregroundColor.R + ", \"green\": " + foregroundColor.G + ", \"blue\": " + foregroundColor.B + "},";
    result = result + "\"backgroundColor\": {\"red\": " + backgroundColor.R + ", \"green\": " + backgroundColor.G + ", \"blue\": " + backgroundColor.B + "},";
    result = result + "\"brightness\": " + brightness + ",";
    result = result + "\"timeZone\": " + timeZone + ",";
    result = result + "\"daylightSavingsTime\": " + (daylightSavingsTime ? "true" : "false") + ",";
    result = result + "\"sleepHour\": " + sleepHour + ",";
    result = result + "\"sleepMinute\": " + sleepMinute + ",";
    result = result + "\"wakeupHour\": " + wakeupHour + ",";
    result = result + "\"wakeupMinute\": " + wakeupMinute + "";
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
        !webServer.hasArg("wakeupTime")
    ) {
        success = false;
    } else {


        foregroundColor = getRGBFromHex(webServer.arg("foregroundColor"));


        backgroundColor = getRGBFromHex(webServer.arg("backgroundColor"));


        brightness = webServer.arg("brightness").toInt();


        timeZone = webServer.arg("timeZone").toInt();


        if (webServer.arg("daylightSavingsTime") == String("true"))
            daylightSavingsTime = true;
        else
            daylightSavingsTime = false;


        sleepHour = split(webServer.arg("sleepTime"), ':', 0).toInt();
        sleepMinute = split(webServer.arg("sleepTime"), ':', 1).toInt();


        wakeupHour = split(webServer.arg("wakeupTime"), ':', 0).toInt();
        wakeupMinute = split(webServer.arg("wakeupTime"), ':', 1).toInt();
    }

    result = result + "{\"success\": " + (success ? "true" : "false") + "}";

    saveEEPROM();

    webServer.send(200, "application/json", result);
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