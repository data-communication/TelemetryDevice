#include "WifiConnection.h"
#include <WiFi.h>

unsigned long WifiConnection::lastRetryTime = 0;
bool WifiConnection::wasConnected = false;

void WifiConnection::initWifi(const DeviceSettings &settings){
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(settings.wifiSSID, settings.wifiPassword);

    lastRetryTime = millis();
    Serial.print(F("Connecting to Wi-Fi: "));
    Serial.println(settings.wifiSSID);
}

void WifiConnection::update(){
    bool connected = WiFi.status() == WL_CONNECTED;

    if (connected && !wasConnected) {
        Serial.print(F("Wi-Fi connected. IP: "));
        Serial.println(WiFi.localIP());
    } else if (!connected && wasConnected) {
        Serial.println(F("Wi-Fi disconnected."));
    }

    wasConnected = connected;

    if (!connected && millis() - lastRetryTime >= RETRY_INTERVAL){
        lastRetryTime = millis();
        WiFi.reconnect();
    }
}

