//
// Created by Carsten Walther.
//

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "main.h"
#include "WiFiManager.h"
#include "ConfigurationManager.h"

WiFiManager wiFiManager;

void WiFiManager::begin(char const *apName)
{
    captivePortalName = apName;

    WiFi.mode(WIFI_STA);

    // set static IP if entered
    ip = IPAddress(configurationManager.internal.ip);
    gw = IPAddress(configurationManager.internal.gw);
    sub = IPAddress(configurationManager.internal.sub);
    dns = IPAddress(configurationManager.internal.dns);

    if (ip.isSet() || gw.isSet() || sub.isSet() || dns.isSet())
    {
        DEBUG_PRINTLN(PSTR("> using static IP"));
        WiFi.config(ip, gw, sub, dns);
    }

    if (WiFi.SSID() != "")
    {
        // trying to fix connection in progress hanging
        ETS_UART_INTR_DISABLE();
        wifi_station_disconnect();
        ETS_UART_INTR_ENABLE();

        WiFi.begin();
        delay(2000);

        DEBUG_PRINTLN(PSTR("> trying to connect to stored WiFi details"));
        DEBUG_PRINT("> connecting ");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            DEBUG_PRINT(".");
        }
        DEBUG_PRINTLN();
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        // connected
        DEBUG_PRINTLN(PSTR("> connected to stored WiFi details"));
        DEBUG_PRINT(PSTR("> ip: "));
        DEBUG_PRINTLN(WiFi.localIP());

        // start the multicast domain name server
        if (MDNS.begin(configurationManager.data.hostname ? configurationManager.data.hostname : SERVER_HOST))
        {
            MDNS.addService("http", "tcp", SERVER_PORT);
            
            DEBUG_PRINT(PSTR("> mdns started at with domain "));
            DEBUG_PRINT((configurationManager.data.hostname ? configurationManager.data.hostname : SERVER_HOST) + String(".local"));
            DEBUG_PRINT(PSTR(" on port "));
            DEBUG_PRINTLN(SERVER_PORT);
        }
    }
    else
    {
        // captive portal
        startCaptivePortal(captivePortalName);
    }
}

void WiFiManager::forget()
{
    WiFi.disconnect();
    
    startCaptivePortal(captivePortalName);

    // remove IP address from EEPROM
    ip = IPAddress();
    sub = IPAddress();
    gw = IPAddress();
    dns = IPAddress();

    // make EEPROM empty
    storeToEEPROM();

    DEBUG_PRINTLN(PSTR("> requested to forget wifi, started captive portal"));
}

void WiFiManager::setNewWifi(String newSSID, String newPass)
{
    ssid = newSSID;
    pass = newPass;
    ip = IPAddress();
    sub = IPAddress();
    gw = IPAddress();
    dns = IPAddress();

    reconnect = true;
}

void WiFiManager::setNewWifi(String newSSID, String newPass, String newIp, String newSub, String newGw, String newDns)
{
    ssid = newSSID;
    pass = newPass;
    ip.fromString(newIp);
    sub.fromString(newSub);
    gw.fromString(newGw);
    dns.fromString(newDns);

    reconnect = true;
}

void WiFiManager::connectNewWifi(String newSSID, String newPass)
{
    delay(1000);

    DEBUG_PRINT(PSTR("> new SSID: "));
    DEBUG_PRINTLN(newSSID);
    DEBUG_PRINT(PSTR("> new Pass: "));
    DEBUG_PRINTLN(newPass);

    // set static IP or zeros if undefined
    WiFi.config(ip, gw, sub, dns);

    // fix for auto connect racing issue
    if (!(WiFi.status() == WL_CONNECTED && (WiFi.SSID() == newSSID)) || ip.v4() != configurationManager.internal.ip || dns.v4() != configurationManager.internal.dns)
    {
        // trying to fix connection in progress hanging
        ETS_UART_INTR_DISABLE();
        wifi_station_disconnect();
        ETS_UART_INTR_ENABLE();

        // store old data in case new network is wrong
        String oldSSID = WiFi.SSID();
        String oldPSK = WiFi.psk();

        WiFi.begin(String(newSSID), String(newPass), 0, NULL, true);
        delay(2000);

        DEBUG_PRINT("> connecting ");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            DEBUG_PRINT(".");
        }
        DEBUG_PRINTLN();

        if (WiFi.status() != WL_CONNECTED)
        {
            DEBUG_PRINTLN(PSTR("> new connection unsuccessful"));

            if (!inCaptivePortal)
            {
                WiFi.begin(oldSSID, oldPSK, 0, NULL, true);
                delay(2000);

                DEBUG_PRINT("> connecting ");
                while (WiFi.status() != WL_CONNECTED)
                {
                    delay(500);
                    DEBUG_PRINT(".");
                }
                DEBUG_PRINTLN();

                if (WiFi.status() != WL_CONNECTED)
                {
                    DEBUG_PRINTLN(PSTR("> reconnection failed too"));
                    startCaptivePortal(captivePortalName);
                }
                else
                {
                    DEBUG_PRINTLN(PSTR("> reconnection successful"));
                    DEBUG_PRINT(PSTR("> ip: "));
                    DEBUG_PRINTLN(WiFi.localIP());
                }
            }
        }
        else
        {
            if (inCaptivePortal)
            {
                stopCaptivePortal();
            }

            DEBUG_PRINTLN(PSTR("> new connection successful"));
            DEBUG_PRINT(PSTR("> ip: "));
            DEBUG_PRINTLN(WiFi.localIP());

            // store IP address in EEProm
            storeToEEPROM();
        }
    }
}

void WiFiManager::startCaptivePortal(char const *apName)
{
    WiFi.persistent(false);
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.persistent(true);

    WiFi.softAP(apName);

    // Setup the DNS server redirecting all the domains to the apIP
    dnsServer = new DNSServer();
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(53, PSTR("*"), WiFi.softAPIP());

    DEBUG_PRINTLN(PSTR("> opened a captive portal"));
    DEBUG_PRINT(PSTR("> ip: "));
    DEBUG_PRINTLN(WiFi.softAPIP());

    inCaptivePortal = true;
}

void WiFiManager::stopCaptivePortal()
{
    WiFi.mode(WIFI_STA);

    dnsServer->stop();
    delete dnsServer;

    inCaptivePortal = false;
}

bool WiFiManager::isCaptivePortal()
{
    return inCaptivePortal;
}

void WiFiManager::loop()
{
    if (inCaptivePortal)
    {
        if (dnsServer)
        {
            // captive portal loop
            dnsServer->processNextRequest();
        }
    }

    if (reconnect)
    {
        connectNewWifi(ssid, pass);
        reconnect = false;
    }
}

void WiFiManager::storeToEEPROM()
{
    configurationManager.internal.ip = ip.v4();
    configurationManager.internal.gw = gw.v4();
    configurationManager.internal.sub = sub.v4();
    configurationManager.internal.dns = dns.v4();
    configurationManager.save();
}

String WiFiManager::getSsid()
{
    return WiFi.SSID();
}

String WiFiManager::getMacAddress()
{
    return WiFi.macAddress();
}

String WiFiManager::getLocalIp()
{
    return WiFi.localIP().toString();
}

String WiFiManager::getGatewayIp()
{
    return WiFi.gatewayIP().toString();
}

String WiFiManager::getSubnetMask()
{
    return WiFi.subnetMask().toString();
}

String WiFiManager::getHostname()
{
    return WiFi.getHostname();
}

String WiFiManager::getDnsIp()
{
    return WiFi.dnsIP().toString();
}

String WiFiManager::scanWiFiNetworks()
{
    String networks;

    int n = WiFi.scanComplete();
    
    if (n == -2)
    {
        WiFi.scanNetworks(true);
    }
    else if (n)
    {
        // sort networks
        int indices[n];

        int counter = 0;

        for (int i = 0; i < n; i++)
        {
            indices[i] = i;
        }

        // old sort
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
                {
                    std::swap(indices[i], indices[j]);
                }
            }
        }

        // remove duplicates (must be RSSI sorted)
        String cssid;
        for (int i = 0; i < n; i++)
        {
            if (indices[i] == -1)
            {
                continue;
            }

            cssid = WiFi.SSID(indices[i]);

            for (int j = i + 1; j < n; j++)
            {
                if (cssid == WiFi.SSID(indices[j]))
                {
                    // set duplicate aps to index -1
                    indices[j] = -1;
                }
            }
        }

        // add networks to list
        for (int i = 0; i < n; ++i)
        {
            // skip duplicates
            if (indices[i] == -1)
            {
                continue;
            }
            else
            {
                counter++;
            }

            networks += "{";
            networks += "\"rssi\":" + String(WiFi.RSSI(indices[i])) + ",";
            networks += "\"ssid\":\"" + WiFi.SSID(indices[i]) + "\",";
            networks += "\"bssid\":\"" + WiFi.BSSIDstr(indices[i]) + "\",";
            networks += "\"channel\":" + String(WiFi.channel(indices[i])) + ",";
            networks += "\"encryptionType\":" + String(WiFi.encryptionType(indices[i])) + ",";
            networks += "\"hidden\":" + String(WiFi.isHidden(indices[i]) ? "true" : "false");
            networks += "},";
        }

        WiFi.scanDelete();

        if (WiFi.scanComplete() == -2)
        {
            WiFi.scanNetworks(true);
        }
    }

    if (networks.length() > 1)
    {
        networks = networks.substring(0, networks.length() - 1);
    }

    return networks;
}
