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

void WiFiManager::begin(char const *apName, unsigned long newTimeout)
{
    captivePortalName = apName;
    timeout = newTimeout;

    WiFi.mode(WIFI_STA);

    // set static IP if entered
    ip = IPAddress(configurationManager.internal.ip);
    gw = IPAddress(configurationManager.internal.gw);
    sub = IPAddress(configurationManager.internal.sub);
    dns = IPAddress(configurationManager.internal.dns);

    // start the multicast domain name server
    if (MDNS.begin(SERVER_HOST))
    {
        MDNS.addService("http", "tcp", SERVER_PORT);
        
        Serial.print(PSTR("> mdns started at with domain "));
        Serial.print(SERVER_HOST + String(".local"));
        Serial.print(PSTR(" on port "));
        Serial.println(SERVER_PORT);
    }

    if (ip.isSet() || gw.isSet() || sub.isSet() || dns.isSet())
    {
        Serial.println(PSTR("> using static IP"));
        WiFi.config(ip, gw, sub, dns);
    }

    if (WiFi.SSID() != "")
    {
        // trying to fix connection in progress hanging
        ETS_UART_INTR_DISABLE();
        wifi_station_disconnect();
        ETS_UART_INTR_ENABLE();
        WiFi.begin();
    }

    if (WiFi.waitForConnectResult(timeout) == WL_CONNECTED)
    {
        // connected
        Serial.println(PSTR("> connected to stored WiFi details"));
        Serial.print(PSTR("> ip: "));
        Serial.println(WiFi.localIP());
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

    Serial.println(PSTR("> requested to forget WiFi, started captive portal."));
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

        WiFi.begin(newSSID.c_str(), newPass.c_str(), 0, NULL, true);
        delay(2000);

        if (WiFi.waitForConnectResult(timeout) != WL_CONNECTED)
        {

            Serial.println(PSTR("> new connection unsuccessful"));
            if (!inCaptivePortal)
            {
                WiFi.begin(oldSSID, oldPSK, 0, NULL, true);
                if (WiFi.waitForConnectResult(timeout) != WL_CONNECTED)
                {
                    Serial.println(PSTR("> reconnection failed too"));
                    startCaptivePortal(captivePortalName);
                }
                else
                {
                    Serial.println(PSTR("> reconnection successful"));
                    Serial.print(PSTR("> ip: "));
                    Serial.println(WiFi.localIP());
                }
            }
        }
        else
        {
            if (inCaptivePortal)
            {
                stopCaptivePortal();
            }

            Serial.println(PSTR("> new connection successful"));
            Serial.print(PSTR("> ip: "));
            Serial.println(WiFi.localIP());

            // store IP address in EEProm
            storeToEEPROM();
        }
    }
}

void WiFiManager::startCaptivePortal(char const *apName)
{
    WiFi.persistent(false);
    // disconnect sta, start ap

    // this alone is not enough to stop the autoconnecter
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.persistent(true);

    WiFi.softAP(apName);

    dnsServer = new DNSServer();

    // Setup the DNS server redirecting all the domains to the apIP
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(53, "*", WiFi.softAPIP());

    Serial.println(PSTR("> opened a captive portal"));
    Serial.print(PSTR("> ip: "));
    Serial.println(WiFi.softAPIP());
    inCaptivePortal = true;
}

void WiFiManager::stopCaptivePortal()
{
    WiFi.mode(WIFI_STA);
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
        //captive portal loop
        dnsServer->processNextRequest();
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
