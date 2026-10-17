#include "WifiConnection.h"
#include <WiFi.h>
#include "rgb_led.h"

#define RECONNECT_BLINK_TIMER 500

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

bool WifiConnection::update(){
    bool connected = WiFi.status() == WL_CONNECTED;
    static unsigned long disconnectTime = 0;
    static unsigned long lastBlink = 0;
    static uint8_t blinkState = 0;

    if(connected && !wasConnected) {
        Serial.print(F("Wi-Fi connected. IP: "));
        Serial.println(WiFi.localIP());
        rgb_green();
    } else if(!connected && wasConnected) {
        Serial.println(F("Wi-Fi disconnected."));
        rgb_red();
        disconnectTime = millis();
    }

    wasConnected = connected;

    if(!connected && millis() - lastBlink >= RECONNECT_BLINK_TIMER && millis() - disconnectTime >= 2000){
        lastBlink = millis();
        blinkState ? rgb_blue() : rgb_off();
        blinkState ^= 1;
    }
    
    if(!connected && millis() - lastRetryTime >= RETRY_INTERVAL){
        lastRetryTime = millis();
        WiFi.reconnect();
    }
    
    return connected;
}

