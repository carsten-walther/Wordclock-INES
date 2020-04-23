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
#include "Color/RGB.h"

// WiFiManager
// Once its business is done, there is no need to keep it around
WiFiManager wifiManager;

// Create a web server on port 80
ESP8266WebServer webServer(AP_PORT);

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
RGB foregroundColor = RGB(225, 0, 0);
// backgroundColor
RGB backgroundColor = RGB(0, 0, 255);
// brightness
int brightness = 10;
// timeZone
int timeZone = 1;
// daylightSavingsTime
bool daylightSavingsTime = true;
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

// =============================================================================

void setup()
{
    #ifdef DEBUG
    Serial.begin(BAUD);
    Serial.println(" ");
    Serial.println("Starting Wordclock INES...");
    Serial.flush();
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
    wifi_station_set_hostname(AP_HOST);
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

        // start the multicast domain name server
        mdnsResponder.begin(AP_HOST);
        // and add dns services
        mdnsResponder.addService("http", "tcp", AP_PORT);

        // start the SPI Flash File System (SPIFFS)
        fs::SPIFFSConfig cfg;
        cfg.setAutoFormat(false);
        SPIFFS.setConfig(cfg);
        SPIFFS.begin();

        // start the ntp time client
        timeClient.begin();
    }
}

// =============================================================================

void loop()
{
    // update the ntp time client
    timeClient.update();

    // run the webServer
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
    }

    ledStrip.setBrightness(brightness);
    ledStrip.show();
}

// =============================================================================

void faceNormal(uint16_t hours, uint16_t minutes)
{

}

void faceNight()
{

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

String getContentType(String filename)
{
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
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
