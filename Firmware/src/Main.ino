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
#include <ArduinoJson.h>
#ifdef USE_OTA
#include <ArduinoOTA.h>
#endif
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// local libs
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
    SETTING_VERSION,// Version
    USE_BASIC_AUTH, // Auth Use
    AUTH_USERNAME,  // Auth Username
    AUTH_PASSWORD   // Auth Password
};

// Settings
Settings settings(defaults);

// clockMode
int clockMode = CLOCKMODE_NORMAL;
int clockModeOverride = -1;

// face timer
unsigned long previousMillis = 0;

// file system status
static bool fsOK;

// =============================================================================

void setup()
{
    #ifdef DEBUG
    Serial.begin(BAUD);
    Serial.println("Starting Wordclock...");
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
        HTTP.on("/settings", HTTP_GET, handleSettingsJson);
        // if update requested update settings and return result
        HTTP.on("/update", HTTP_POST, handleUpdateJson);
        // if description.xml return description
        HTTP.on("/description.xml", HTTP_GET, handleSSDP);
        // if status return fs status
        HTTP.on("/status", HTTP_GET, handleStatus);

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
        if ((fsOK = fileSystem->begin()) != false) {
            fileSystemConfig.setAutoFormat(false);
            fileSystem->setConfig(fileSystemConfig);
            #ifdef DEBUG
            Serial.println(fsOK ? "Filesystem initialized." : "Filesystem init failed!");
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

        // process OTA
        initOTA();
    }

    // start the ntp time client
    NTP.begin();

    // process time offsets
    processTimeOffset();

    #ifdef DEBUG
    Serial.println("Setup complete");
    #endif
}

// =============================================================================

void loop()
{
    // update the ntp time client
    NTP.update();

    // update MDNS service
    MDNS.update();

    // handle HTTP client
    HTTP.handleClient();

    // handle OTA
    #ifdef USE_OTA
    ArduinoOTA.handle();
    #endif

    // Set default clockMode
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

void faceScanner()
{
    unsigned int speedDelay = 125;

    if (millis() - previousMillis >= speedDelay) {

        previousMillis = millis();

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
    unsigned int speedDelay = 125;

    if (millis() - previousMillis >= speedDelay) {

        previousMillis = millis();

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
    if (millis() - previousMillis >= 1000) {

        previousMillis = millis();

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
    if (millis() - previousMillis >= 1000) {

        previousMillis = millis();

        ledStrip.setBrightness(0);
        ledStrip.clear();
        ledStrip.show();
    }
}

// =============================================================================

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

// =============================================================================

void initOTA()
{
    // init OTA
    #ifdef USE_OTA

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(SERVER_HOST);

    // Port defaults to 8266
    #ifdef OTA_PORT
    ArduinoOTA.setPort(OTA_PORT);
    #endif

    // No authentication by default
    #ifdef OTA_PASS
    ArduinoOTA.setPassword(OTA_PASS);
    #endif

    ArduinoOTA.onStart([]() {
        #ifdef DEBUG
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
        #endif
    });

    ArduinoOTA.onEnd([]() {
        #ifdef DEBUG
        Serial.println("\nEnd");
        #endif
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        #ifdef DEBUG
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        #endif
    });

    ArduinoOTA.onError([](ota_error_t error) {
        #ifdef DEBUG
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
        #endif
    });

    ArduinoOTA.begin();
    #ifdef DEBUG
    Serial.println("ArduinoOTA started");
    #endif

    #endif
}

// =============================================================================

void handleAuth()
{
    if (settings.parameters->useAuth) {
        if (!HTTP.authenticate(string2char(settings.parameters->authUsername), string2char(settings.parameters->authPassword))) {
            return HTTP.requestAuthentication(DIGEST_AUTH, AUTH_REALM, "Error 401: Authentication Failed");
        }
    }
}

void handleNotFound()
{
    handleAuth();

    // check if the file exists in the flash memory (LittleFS), if so, send it
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

    // If a folder is requested, send the index file
    if (path.endsWith("/")) {
        path += "index.html";
    }

    // Get the MIME type
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";

    // If the file exists, either as a compressed archive, or normal
    if (fileSystem->exists(pathWithGz) || fileSystem->exists(path)) {

        // If there's a compressed version available
        if (fileSystem->exists(pathWithGz)) {
            // Use the compressed verion
            path += ".gz";
        }

        // Open the file
        File file = fileSystem->open(path, "r");
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

    // Read JSON from plain
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
        settings.parameters->daylightSavingsTime = obj["daylightSavingsTime"].as<bool>();
    }

    if (obj["sleepTime"]) {
        success = true;
        // sleepTime (sleepHour, sleepMinute)
        settings.parameters->sleepHour = split(obj["sleepTime"], ':', 0).toInt();
        settings.parameters->sleepMinute = split(obj["sleepTime"], ':', 1).toInt();
    }

    if (obj["wakeupTime"]) {
        success = true;
        // wakeupTime (wakeupHour, wakeupMinute)
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

    if (obj["useAuth"]) {
        success = true;
        settings.parameters->useAuth = obj["useAuth"].as<bool>();
    }

    if (obj["authUsername"]) {
        success = true;
        settings.parameters->authUsername = obj["authUsername"].as<const char*>();
    }

    if (obj["authPassword"]) {
        success = true;
        settings.parameters->authPassword = obj["authPassword"].as<const char*>();
    }

    // save settings
    settings.save();

    // process time offsets
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
    // List the file system contents
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

    res["result"]["useAuth"] = settings.parameters->useAuth;
    res["result"]["authUsername"] = settings.parameters->authUsername;
    res["result"]["authPassword"] = settings.parameters->authPassword;

    String output;
    serializeJson(res, output);
    return output;
}

String getContentType(String filename)
{
    if (filename.endsWith(".html")) {
        return "text/html";
    } else if (filename.endsWith(".css")) {
        return "text/css";
    } else if (filename.endsWith(".js")) {
        return "application/javascript";
    } else if (filename.endsWith(".png")) {
        return "image/png";
    } else if (filename.endsWith(".jpg")) {
        return "image/jpeg";
    } else if (filename.endsWith(".svg")) {
        return "image/svg+xml";
    } else if (filename.endsWith(".ico")) {
        return "image/x-icon";
    } else if (filename.endsWith(".json")) {
        return "application/json";
    } else if (filename.endsWith(".gz")) {
        return "application/x-gzip";
    } else {
        return "text/plain";
    }
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

char* string2char(String str)
{
    if (str.length() != 0) {
        char *p = const_cast<char*>(str.c_str());
        return p;
    }
}
