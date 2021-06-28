//
// Created by Carsten Walther.
//

#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>

#include "main.h"
#include "Updater.h"
#include "WebServer.h"
#include "WiFiManager.h"
#include "ConfigurationManager.h"

WebServer webServer;

void WebServer::begin()
{
    // to enable testing and debugging of the interface
    DefaultHeaders::Instance().addHeader(PSTR("Access-Control-Allow-Origin"), PSTR("*"));

    // start the SPI Flash File System (LittleFS)
    if (LittleFS.begin() != false)
    {
        Serial.println(PSTR("> filesystem initialized"));
    }
    else
    {
        Serial.println(PSTR("> filesystem init failed"));
    }

    server.begin();

    if (configurationManager.data.useAuth)
    {
        server
            .serveStatic(PSTR("/"), LittleFS, PSTR("/"))
            .setDefaultFile("index.html")
            .setAuthentication(configurationManager.data.authUsername, configurationManager.data.authPassword);
    }
    else
    {
        server
            .serveStatic(PSTR("/"), LittleFS, PSTR("/"))
            .setDefaultFile("index.html");
    }

    // handle not found and captive portal
    // server.onNotFound(handleNotFound);

    bindAll();

    if (!wiFiManager.isCaptivePortal())
    {
        // init SSDP
        SSDP.setSchemaURL("description.xml");
        SSDP.setHTTPPort(SERVER_PORT);
        SSDP.setName(configurationManager.data.hostname);
        SSDP.setSerialNumber(ESP.getChipId());
        SSDP.setURL("/");
        SSDP.setModelName(AP_SSID);
        SSDP.setModelNumber(VERSION);
        SSDP.setModelURL("https://github.com/carsten-walther/wordclock");
        SSDP.setManufacturer("Carsten Walther");
        SSDP.setManufacturerURL("https://www.carstenwalther.de");
        SSDP.setDeviceType("upnp:rootdevice");

        if (SSDP.begin())
        {
            Serial.println(PSTR("> ssdp started"));
        }

        // handle SSDP
        server.on(PSTR("/description.xml"), HTTP_GET, handleSimpleServiceDiscoveryProtocol);
    }
}

void WebServer::loop()
{
    if (!wiFiManager.isCaptivePortal())
    {
        // update MDNS service
        MDNS.update();
    }
}

void WebServer::bindAll()
{
    
    //
    // WIFI
    //

    // get WiFi details
    server.on(PSTR("/api/wifi/get"), HTTP_GET, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(256);

        doc["success"] = true;

        doc["payload"]["ssid"] = wiFiManager.getSsid();
        doc["payload"]["ip"] = wiFiManager.getLocalIp();
        doc["payload"]["gw"] = wiFiManager.getGatewayIp();
        doc["payload"]["dns"] = wiFiManager.getDnsIp();
        doc["payload"]["sub"] = wiFiManager.getSubnetMask();
        doc["payload"]["captivePortal"] = wiFiManager.isCaptivePortal();

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);
    });

    // update WiFi details
    server.on(PSTR("/api/wifi/set"), HTTP_POST, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(32);

        doc["success"] = true;

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);

        wiFiManager.setNewWifi(
            request->getParam("ssid", true)->value().c_str(), 
            request->getParam("pass", true)->value().c_str()
        );
    });

     // update WiFi details with static IP
    server.on(PSTR("/api/wifi/setStatic"), HTTP_POST, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(32);

        doc["success"] = true;

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);

        wiFiManager.setNewWifi(
            request->getParam("ssid", true)->value().c_str(), 
            request->getParam("pass", true)->value().c_str(), 
            request->getParam("ip", true)->value().c_str(), 
            request->getParam("sub", true)->value().c_str(), 
            request->getParam("gw", true)->value().c_str(), 
            request->getParam("dns", true)->value().c_str()
        );
    });

    // scan WiFi networks
    server.on(PSTR("/api/wifi/scan"), HTTP_GET, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(4096);

        doc["success"] = true;

        String networksString = wiFiManager.scanWiFiNetworks();
        if (networksString.length() > 0)
        {
            JsonArray networks = doc["payload"].createNestedArray("networks");
            networks.add(serialized(networksString));
        }
        else
        {
            doc["payload"]["networks"] = "";
        }

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);
    });

    // forget WiFi details
    server.on(PSTR("/api/wifi/forget"), HTTP_POST, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(32);

        doc["success"] = true;

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);

        wiFiManager.forget();
    });

    //
    // FILES
    //

    // get file listing
    server.on(PSTR("/api/files/get"), HTTP_GET, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(4096);

        doc["success"] = true;

        // get file listing
        int counter = 0;
        Dir dir = LittleFS.openDir("/");

        while (dir.next())
        {
            File file = dir.openFile("r");
            doc["payload"]["files"][counter]["name"] = String(file.name());
            doc["payload"]["files"][counter]["size"] = file.size();

            counter++;
        }

        // get used and total data
        FSInfo fs_info;
        LittleFS.info(fs_info);

        doc["payload"]["usedBytes"] = fs_info.usedBytes;
        doc["payload"]["totalBytes"] = fs_info.totalBytes;

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);
    });

    // remove file
    server.on(PSTR("/api/files/remove"), HTTP_POST, [](AsyncWebServerRequest *request)
    {
        String filename = request->getParam("filename", true)->value().c_str();

        Serial.print(PSTR("> deleting file "));
        Serial.println(filename);

        if (!filename.startsWith("/"))
        {
            filename = "/" + filename;
        }

        LittleFS.remove(filename);

        DynamicJsonDocument doc(32);

        doc["success"] = !LittleFS.exists(filename);

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);
    });

    // handle uploads
    server.on(PSTR("/api/files/upload"), HTTP_POST, [](AsyncWebServerRequest *request) {}, handleFileUpload);

    //
    // SYSTEM
    //

    // restart the ESP
    server.on(PSTR("/api/system/restart"), HTTP_POST, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(32);

        doc["success"] = true;

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);

        ESP.restart();
    });

    // info
    server.on(PSTR("/api/system/info"), HTTP_GET, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(128);

        doc["success"] = true;

        doc["payload"]["version"] = VERSION;
        doc["payload"]["mac"] = wiFiManager.getMacAddress();

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);
    });

    // update from FS
    server.on(PSTR("/api/system/update"), HTTP_POST, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(32);

        doc["success"] = true;

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);

        updater.requestStart("/" + request->arg("filename"));
    });

    // update status
    server.on(PSTR("/api/system/update-status"), HTTP_GET, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(64);

        doc["success"] = true;
        doc["payload"]["status"] = updater.getStatus();

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);
    });

    //
    // CONFIG
    //

    // send configuration data
    server.on(PSTR("/api/config/get"), HTTP_GET, [](AsyncWebServerRequest *request)
    {
        DynamicJsonDocument doc(2048);

        doc["success"] = true;

        doc["payload"]["foreground"]["r"] = configurationManager.data.foregroundRed;
        doc["payload"]["foreground"]["g"] = configurationManager.data.foregroundGreen;
        doc["payload"]["foreground"]["b"] = configurationManager.data.foregroundBlue;
        doc["payload"]["background"]["r"] = configurationManager.data.backgroundRed;
        doc["payload"]["background"]["g"] = configurationManager.data.backgroundGreen;
        doc["payload"]["background"]["b"] = configurationManager.data.backgroundBlue;
        doc["payload"]["mode"] = configurationManager.data.mode;
        doc["payload"]["brightness"] = configurationManager.data.brightness;
        doc["payload"]["sleep"]["h"] = configurationManager.data.sleepHour;
        doc["payload"]["sleep"]["m"] = configurationManager.data.sleepMinute;
        doc["payload"]["wakeup"]["h"] = configurationManager.data.wakeupHour;
        doc["payload"]["wakeup"]["m"] = configurationManager.data.wakeupMinute;
        doc["payload"]["language"] = configurationManager.data.language;
        doc["payload"]["timezone"] = configurationManager.data.timezone;
        doc["payload"]["daylightSavingTime"] = configurationManager.data.daylightSavingTime;
        doc["payload"]["ntpSyncInterval"] = configurationManager.data.ntpSyncInterval;
        doc["payload"]["ntpServer"] = configurationManager.data.ntpServer;
        doc["payload"]["hostname"] = configurationManager.data.hostname;
        doc["payload"]["useAuth"] = configurationManager.data.useAuth;
        doc["payload"]["authUsername"] = configurationManager.data.authUsername;
        doc["payload"]["authPassword"] = configurationManager.data.authPassword;

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);
    });

    // receive configuration data from body
    server.on(PSTR("/api/config/set"), HTTP_POST, [](AsyncWebServerRequest *request)
    {
        if (request->hasParam("foregroundRed", true))
            configurationManager.data.foregroundRed = (int)request->getParam("foregroundRed", true)->value().toInt();
        if (request->hasParam("foregroundGreen", true))
            configurationManager.data.foregroundGreen = (int)request->getParam("foregroundGreen", true)->value().toInt();
        if (request->hasParam("foregroundBlue", true))
            configurationManager.data.foregroundBlue = (int)request->getParam("foregroundBlue", true)->value().toInt();
        if (request->hasParam("backgroundRed", true))
            configurationManager.data.backgroundRed = (int)request->getParam("backgroundRed", true)->value().toInt();
        if (request->hasParam("backgroundGreen", true))
            configurationManager.data.backgroundGreen = (int)request->getParam("backgroundGreen", true)->value().toInt();
        if (request->hasParam("backgroundBlue", true))
            configurationManager.data.backgroundBlue = (int)request->getParam("backgroundBlue", true)->value().toInt();
        if (request->hasParam("mode", true))
            configurationManager.data.mode = (int)request->getParam("mode", true)->value().toInt();
        if (request->hasParam("brightness", true))
            configurationManager.data.brightness = (int)request->getParam("brightness", true)->value().toInt();
        if (request->hasParam("sleepHour", true))
            configurationManager.data.sleepHour = (int)request->getParam("sleepHour", true)->value().toInt();
        if (request->hasParam("sleepMinute", true))
            configurationManager.data.sleepMinute = (int)request->getParam("sleepMinute", true)->value().toInt();
        if (request->hasParam("wakeupHour", true))
            configurationManager.data.wakeupHour = (int)request->getParam("wakeupHour", true)->value().toInt();
        if (request->hasParam("wakeupMinute", true))
            configurationManager.data.wakeupMinute = (int)request->getParam("wakeupMinute", true)->value().toInt();
        if (request->hasParam("language", true))
            configurationManager.data.language = (int)request->getParam("language", true)->value().toInt();
        if (request->hasParam("timezone", true))
            configurationManager.data.timezone = (int)request->getParam("timezone", true)->value().toInt();
        if (request->hasParam("daylightSavingTime", true))
            configurationManager.data.daylightSavingTime = (request->getParam("daylightSavingTime", true)->value() == "true") ? true : false;
        if (request->hasParam("ntpSyncInterval", true))
            configurationManager.data.ntpSyncInterval = (int)request->getParam("ntpSyncInterval", true)->value().toInt();
        if (request->hasParam("ntpServer", true))
            strcpy(configurationManager.data.ntpServer, request->getParam("ntpServer", true)->value().c_str());
        if (request->hasParam("hostname", true))
            strcpy(configurationManager.data.hostname, request->getParam("hostname", true)->value().c_str());
        if (request->hasParam("useAuth", true))
            configurationManager.data.useAuth = (request->getParam("useAuth", true)->value() == "true") ? true : false;
        if (request->hasParam("authUsername", true))
            strcpy(configurationManager.data.authUsername, request->getParam("authUsername", true)->value().c_str());
        if (request->hasParam("authPassword", true))
            strcpy(configurationManager.data.authPassword, request->getParam("authPassword", true)->value().c_str());

        configurationManager.save();

        DynamicJsonDocument doc(32);

        doc["success"] = true;

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);
    });
}

void WebServer::handleAuth(AsyncWebServerRequest *request)
{
    if (configurationManager.data.useAuth && !request->authenticate(configurationManager.data.authUsername, configurationManager.data.authPassword))
    {
        return request->requestAuthentication(AUTH_REALM);
    }
}

void WebServer::handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    {
        Serial.println(PSTR("> starting file upload"));
        Serial.print(PSTR("> file: "));
        Serial.println(filename);

        // open the file on first call and store the file handle in the request object
        request->_tempFile = LittleFS.open(filename, "w");
    }

    if (len)
    {
        // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);
    }

    if (final)
    {
        // close the file handle as the upload is now done
        request->_tempFile.close();

        Serial.println(PSTR("> finished file upload"));

        DynamicJsonDocument doc(512);
        doc["success"] = final;

        String JSON;
        serializeJson(doc, JSON);
        request->send(200, PSTR("application/json"), JSON);
    }
}

void WebServer::handleNotFound(AsyncWebServerRequest *request)
{
    Serial.print(PSTR("> requested URL: "));
    Serial.println(request->url().c_str());

    if (request->method() == HTTP_GET || request->method() == HTTP_POST)
    {
        request->send(LittleFS, "index.html");
    }
    else if (request->method() == HTTP_OPTIONS)
    {
        request->send(200);
    }
    else
    {
        request->send(404, "text/html", "<h1>Not found</h1><p>The requested URL " + String(request->url().c_str()) + " was not found on this server.</p>");
    }
}

void WebServer::handleSimpleServiceDiscoveryProtocol(AsyncWebServerRequest *request)
{
    static const char* ssdpTemplate = 
        "<?xml version=\"1.0\"?>"
        "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
            "<specVersion>"
            "<major>1</major>"
            "<minor>0</minor>"
            "</specVersion>"
            "<URLBase>http://%s/</URLBase>"
            "<device>"
                "<deviceType>%s</deviceType>"
                "<friendlyName>%s</friendlyName>"
                "<presentationURL>%s</presentationURL>"
                "<serialNumber>%u</serialNumber>"
                "<modelName>%s</modelName>"
                "<modelNumber>%s</modelNumber>"
                "<modelURL>%s</modelURL>"
                "<manufacturer>%s</manufacturer>"
                "<manufacturerURL>%s</manufacturerURL>"
                "<UDN>uuid:b8b8f06a-8301-4675-ba8e-3c11b1%02x%02x%02x</UDN>"
            "</device>"
        "</root>\r\n"
        "\r\n";

    StreamString output;

    if (output.reserve(1024))
    {
        uint32_t chipId = ESP.getChipId();
        output.printf(ssdpTemplate,
            (configurationManager.data.hostname ? configurationManager.data.hostname : SERVER_HOST) + String(".local"), // URLBase
            "upnp:rootdevice",                              // deviceType
            AP_SSID,                                        // friendlyName
            "/",                                            // presentationURL
            chipId,                                         // serialNumber
            AP_SSID,                                        // modelName
            VERSION,                                        // modelNumber
            "https://github.com/carsten-walther/wordclock", // modelURL
            "Carsten Walther",                              // manufacturer
            "https://www.carstenwalther.de",                // manufacturerURL
            (uint8_t) ((chipId >> 16) & 0xff),              // UDN
            (uint8_t) ((chipId >>  8) & 0xff),              // UDN
            (uint8_t)   chipId        & 0xff                // UDN
        );

        request->send(200, "text/xml", (String)output);
    }
    else
    {
        request->send(500);
    }
}