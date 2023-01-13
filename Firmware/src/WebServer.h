//
// Created by Carsten Walther.
//

#ifndef WORDCLOCK_WEBSERVER_H
#define WORDCLOCK_WEBSERVER_H

#include <StreamString.h>
#include <ESPAsyncWebServer.h>

class WebServer
{
private:
    AsyncWebServer server = AsyncWebServer(80);

    void bindAll();
    void handleAuth(AsyncWebServerRequest *request);
    static void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
    static void handleNotFound(AsyncWebServerRequest *request);
    static void handleSimpleServiceDiscoveryProtocol(AsyncWebServerRequest *request);

public:
    void begin();
    void loop();
};

extern WebServer webServer;

#endif // WORDCLOCK_WEBSERVER_H
