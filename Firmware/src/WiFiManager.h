//
// Created by Carsten Walther.
//

#ifndef WORDCLOCK_WIFIMANAGER_H
#define WORDCLOCK_WIFIMANAGER_H

#include <Arduino.h>
#include <DNSServer.h>
#include <memory>

class WiFiManager
{
private:
    DNSServer *dnsServer;
    String ssid;
    String pass;
    IPAddress ip;
    IPAddress gw;
    IPAddress sub;
    IPAddress dns;
    bool reconnect = false;
    bool inCaptivePortal = false;
    char const *captivePortalName;
    unsigned long timeout = 60000;
    
    void startCaptivePortal(char const *apName);
    void stopCaptivePortal();
    void connectNewWifi(String newSSID, String newPass);    
    void storeToEEPROM();
    int8_t waitForConnectResult(unsigned long timeoutLength);
    std::function<void()> _forgetwificallback;
    std::function<void()> _newwificallback;  

public:
    void begin(char const *apName, unsigned long newTimeout = 60000);
    void loop();
    void forget();
    bool isCaptivePortal();
    String SSID();
    void setNewWifi(String newSSID, String newPass);
    void setNewWifi(String newSSID, String newPass, String newIp, String newSub, String newGw, String newDns);
    void forgetWiFiFunctionCallback( std::function<void()> func );
    void newWiFiFunctionCallback( std::function<void()> func );

    String getSsid();
    String getMacAddress();
    String getLocalIp();
    String getGatewayIp();
    String getSubnetMask();
    String getDnsIp();
    String getHostname();
    String scanWiFiNetworks();
};

extern WiFiManager wiFiManager;

#endif // WORDCLOCK_WIFIMANAGER_H
