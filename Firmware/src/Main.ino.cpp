# 1 "/var/folders/3v/f4hyz79x6j7c8cbpn1l0smjh0000gn/T/tmpxkg2n8l7"
#include <Arduino.h>
# 1 "/Users/carstenwalther/Desktop/wordclock.git/Firmware/src/Main.ino"
# 11 "/Users/carstenwalther/Desktop/wordclock.git/Firmware/src/Main.ino"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <NTPClient.h>


#include "Config.h"
#include "Timezones.h"
#include "Settings/Settings.h"

#if defined USE_SPIFFS

#include <FS.h>
const char* fsName = "SPIFFS";
FS* fileSystem = &SPIFFS;
SPIFFSConfig fileSystemConfig = SPIFFSConfig();

#elif defined USE_LITTLEFS

#include <LittleFS.h>
const char* fsName = "LittleFS";
FS* fileSystem = &LittleFS;
LittleFSConfig fileSystemConfig = LittleFSConfig();

#elif defined USE_SDFS

#include <SDFS.h>
const char* fsName = "SDFS";
FS* fileSystem = &SDFS;
SDFSConfig fileSystemConfig = SDFSConfig();

#else
#error Please select a filesystem first by uncommenting one of the "#define USE_xxx" lines at the beginning of the sketch.
#endif



WiFiManager wifiManager;


ESP8266WebServer HTTP(SERVER_PORT);


WiFiUDP UDP;




NTPClient NTP = NTPClient(UDP, NTP_HOST_NAME, NTP_TIME_OFFSET, NTP_UPDATE_INTERVAL);
# 74 "/Users/carstenwalther/Desktop/wordclock.git/Firmware/src/Main.ino"
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


int clockMode = CLOCKMODE_NORMAL;
int clockModeOverride = -1;


unsigned long previousMillis = 0;


static bool fsOK;


const char* www_username = "admin";
const char* www_password = "esp8266";
const char* www_realm = "Wordclock";
void setup();
void loop();
void faceScanner();
void faceTest();
void faceNormal(uint16_t hours, uint16_t minutes);
void faceNight();
String split(String data, char separator, int index);
void processTimeOffset();
void handleAuth();
void handleNotFound();
bool handleFileRead(String path);
void handleSettingsJson();
void handleUpdateJson();
void handleSSDP();
void handleStatus();
String settingsWithSuccess(bool success);
String getContentType(String filename);
String formatBytes(size_t bytes);
#line 118 "/Users/carstenwalther/Desktop/wordclock.git/Firmware/src/Main.ino"
void setup()
{
    #ifdef DEBUG
    Serial.begin(BAUD);
    Serial.println(F("Starting Wordclock..."));
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



        HTTP.onNotFound(handleNotFound);

        HTTP.on("/settings", HTTP_GET, handleSettingsJson);

        HTTP.on("/update", HTTP_POST, handleUpdateJson);

        HTTP.on("/description.xml", HTTP_GET, handleSSDP);

        HTTP.on("/status", HTTP_GET, handleStatus);


        HTTP.begin();
        #ifdef DEBUG
        Serial.println(F("HTTP started"));
        #endif


        if (MDNS.begin(SERVER_HOST)) {
            MDNS.addService("http", "tcp", SERVER_PORT);
            #ifdef DEBUG
            Serial.println(F("MDNS started"));
            #endif
        }


        if (fsOK = fileSystem->begin()) {
            fileSystemConfig.setAutoFormat(false);
            fileSystem->setConfig(fileSystemConfig);
            #ifdef DEBUG
            Serial.println(fsOK ? F("Filesystem initialized.") : F("Filesystem init failed!"));
            #endif
        }


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
            Serial.println(F("SSDP started"));
            #endif
        }
    }


    NTP.begin();


    processTimeOffset();

    #ifdef DEBUG
    Serial.println(F("Setup complete"));
    #endif
}



void loop()
{

    NTP.update();


    MDNS.update();


    HTTP.handleClient();


    clockMode = CLOCKMODE_NORMAL;


    if ((NTP.getHours() == settings.parameters->sleepHour && NTP.getMinutes() >= settings.parameters->sleepMinute) || (NTP.getHours() == settings.parameters->wakeupHour && NTP.getMinutes() < settings.parameters->wakeupMinute)) {
        clockMode = CLOCKMODE_NIGHT;
    }


    if (settings.parameters->sleepHour < settings.parameters->wakeupHour && NTP.getHours() >= settings.parameters->sleepHour && NTP.getHours() <= settings.parameters->wakeupHour) {
        clockMode = CLOCKMODE_NIGHT;
    }


    if (settings.parameters->sleepHour > settings.parameters->wakeupHour && (NTP.getHours() >= settings.parameters->sleepHour || NTP.getHours() <= settings.parameters->wakeupHour)) {
        clockMode = CLOCKMODE_NIGHT;
    }


    if (clockModeOverride > -1) {
        clockMode = clockModeOverride;
    }


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



void faceScanner()
{
    unsigned int speedDelay = 125;

    if (millis() - previousMillis >= speedDelay) {

        previousMillis = millis();


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

    if (millis() - previousMillis >= speedDelay) {

        previousMillis = millis();


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
    if (millis() - previousMillis >= 1000) {

        previousMillis = millis();

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
    if (millis() - previousMillis >= 1000) {

        previousMillis = millis();

        ledStrip.setBrightness(0);
        ledStrip.clear();
        ledStrip.show();
    }
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


    NTP.setTimeOffset(offset);

    NTP.forceUpdate();
}



void handleAuth()
{
    #ifdef USE_BASIC_AUTH
    if (!HTTP.authenticate(www_username, www_password)) {







        return HTTP.requestAuthentication(DIGEST_AUTH, www_realm, "Error 401: Authentication Failed");
    }
    #endif
}

void handleNotFound()
{
    handleAuth();


    if (!handleFileRead(HTTP.uri())) {

        String message = "Error 404: File Not Found\n\n";
     message += "\nURI: ";
     message += HTTP.uri();
     message += "\nMethod: ";
     message += (HTTP.method() == HTTP_GET) ? "GET" : "POST";
     message += "\nArguments: ";
     message += HTTP.args();
     message += "\n";

     for (uint8_t i = 0; i < HTTP.args(); i++) {
      message += "    " + HTTP.argName(i) + ": " + HTTP.arg(i) + "\n";
     }

        HTTP.send(404, "text/plain", message);
    }
}

bool handleFileRead(String path)
{
    if (!fsOK) {
        HTTP.send(500, "text/plain", "Error 500: File system error\r\n");
        return true;
    }


    if (path.endsWith("/")) {
        path += "index.html";
    }


    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";


    if (fileSystem->exists(pathWithGz) || fileSystem->exists(path)) {


        if (fileSystem->exists(pathWithGz)) {

            path += ".gz";
        }


        File file = fileSystem->open(path, "r");

        HTTP.streamFile(file, contentType);

        file.close();

        return true;
    }


    return false;
}

void handleSettingsJson()
{
    handleAuth();

    HTTP.sendHeader("Access-Control-Allow-Origin", "*");
    HTTP.sendHeader("Access-Control-Max-Age", "10000");
    HTTP.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
    HTTP.sendHeader("Access-Control-Allow-Headers", "*");

    HTTP.send(200, "application/json", settingsWithSuccess(true));
}

void handleUpdateJson()
{
    handleAuth();

    bool success = false;


    DynamicJsonDocument doc(1024);
    deserializeJson(doc, HTTP.arg("plain"));
    JsonObject obj = doc.as<JsonObject>();

    if (obj["foregroundColor"]) {
        success = true;
        settings.parameters->foregroundColorRed = obj["foregroundColor"]["red"];
        settings.parameters->foregroundColorGreen = obj["foregroundColor"]["green"];
        settings.parameters->foregroundColorBlue = obj["foregroundColor"]["blue"];
    }

    if (obj["backgroundColor"]) {
        success = true;
        settings.parameters->backgroundColorRed = obj["backgroundColor"]["red"];
        settings.parameters->backgroundColorGreen = obj["backgroundColor"]["green"];
        settings.parameters->backgroundColorBlue = obj["backgroundColor"]["blue"];
    }

    if (obj["brightness"]) {
        success = true;
        settings.parameters->brightness = map(obj["brightness"], 0, 100, 0, 255);
    }

    if (obj["timeZone"]) {
        success = true;
        settings.parameters->timeZone = obj["timeZone"];
    }

    if (obj["daylightSavingsTime"]) {
        success = true;
        settings.parameters->daylightSavingsTime = obj["daylightSavingsTime"];
    }

    if (obj["sleepTime"]) {
        success = true;

        settings.parameters->sleepHour = split(obj["sleepTime"], ':', 0).toInt();
        settings.parameters->sleepMinute = split(obj["sleepTime"], ':', 1).toInt();
    }

    if (obj["wakeupTime"]) {
        success = true;

        settings.parameters->wakeupHour = split(obj["wakeupTime"], ':', 0).toInt();
        settings.parameters->wakeupMinute = split(obj["wakeupTime"], ':', 1).toInt();
    }

    if (obj["language"]) {
        success = true;
        settings.parameters->language = obj["language"];
    }

    if (obj["clockMode"]) {
        success = true;
        clockModeOverride = obj["clockMode"];
    }


    settings.save();


    processTimeOffset();

    HTTP.sendHeader("Access-Control-Allow-Origin", "*");
    HTTP.sendHeader("Access-Control-Max-Age", "10000");
    HTTP.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
    HTTP.sendHeader("Access-Control-Allow-Headers", "*");

    HTTP.send(200, "application/json", settingsWithSuccess(success));
}

void handleSSDP()
{
    SSDP.schema(HTTP.client());
}

void handleStatus()
{
    handleAuth();

    DynamicJsonDocument res(1024);

    FSInfo fs_info;

    res["isOk"] = fsOK;
    res["fileSystemName"] = fsName;
    res["totalBytes"] = formatBytes(fs_info.totalBytes);
    res["usedBytes"] = formatBytes(fs_info.usedBytes);
    res["blockSize"] = fs_info.blockSize;
    res["pageSize"] = fs_info.pageSize;
    res["maxOpenFiles"] = fs_info.maxOpenFiles;
    res["maxPathLength"] = fs_info.maxPathLength;

    Dir dir = fileSystem->openDir("/");

    int i = 0;
    while (dir.next()) {
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        res["contents"][i]["fileName"] = fileName;
        res["contents"][i]["fileSize"] = formatBytes(fileSize);
        i++;
    }

    String output;
    serializeJson(res, output);

    HTTP.send(200, "application/json", output);
}

String settingsWithSuccess(bool success)
{
    DynamicJsonDocument res(1024);

    res["success"] = success;

    res["result"]["foregroundColor"]["red"] = settings.parameters->foregroundColorRed;
    res["result"]["foregroundColor"]["green"] = settings.parameters->foregroundColorGreen;
    res["result"]["foregroundColor"]["blue"] = settings.parameters->foregroundColorBlue;

    res["result"]["backgroundColor"]["red"] = settings.parameters->backgroundColorRed;
    res["result"]["backgroundColor"]["green"] = settings.parameters->backgroundColorGreen;
    res["result"]["backgroundColor"]["blue"] = settings.parameters->backgroundColorBlue;

    res["result"]["brightness"] = map(settings.parameters->brightness, 0, 255, 0, 100);
    res["result"]["timeZone"] = settings.parameters->timeZone;
    res["result"]["daylightSavingsTime"] = settings.parameters->daylightSavingsTime;
    res["result"]["sleepHour"] = settings.parameters->sleepHour;
    res["result"]["sleepMinute"] = settings.parameters->sleepMinute;
    res["result"]["wakeupHour"] = settings.parameters->wakeupHour;
    res["result"]["wakeupMinute"] = settings.parameters->wakeupMinute;
    res["result"]["language"] = settings.parameters->language;
    res["result"]["clockMode"] = clockMode;
    res["result"]["version"] = VERSION;
    res["result"]["settings"] = SETTING_VERSION;
    res["result"]["time"] = NTP.getFormattedTime();

    String output;
    serializeJson(res, output);
    return output;
}

String getContentType(String filename)
{
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".svg")) return "image/svg+xml";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".json")) return "application/json";
    else if (filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}

String formatBytes(size_t bytes)
{
    if (bytes < 1024) {
        return String(bytes) + "B";
    } else if (bytes < (1024 * 1024)) {
        return String(bytes / 1024.0) + "KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    } else {
        return String("");
    }
}