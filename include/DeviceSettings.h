#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <string.h>

typedef struct {
    char deviceId[37];
    char deviceLocation[33];
    uint16_t measurementInterval;
    
    char wifiSSID[33];
    char wifiPassword[65];

    char mqttUsername[33];
    char mqttPassword[65];
    char mqttHost[33];
    char mqttTopic[96];
    uint16_t mqttPort;
} DeviceSettings;

template <size_t N>
bool hasTerminator(const char (&value)[N]) {
    return memchr(value, '\0', N) != nullptr;
}

inline bool isValidPort(const uint16_t port){
    return port == 1883 || port == 8883;
}

inline bool validSettings(const DeviceSettings& settings){
    if (!hasTerminator(settings.deviceId) ||
        !hasTerminator(settings.deviceLocation) ||
        !hasTerminator(settings.wifiSSID) ||
        !hasTerminator(settings.wifiPassword) ||
        !hasTerminator(settings.mqttUsername) ||
        !hasTerminator(settings.mqttPassword) ||
        !hasTerminator(settings.mqttHost) ||
        !hasTerminator(settings.mqttTopic)
    ){
        return false;
    }

    if (settings.deviceId[0] == '\0' ||
        settings.deviceLocation[0] == '\0' ||
        settings.wifiSSID[0] == '\0' ||
        settings.wifiPassword[0] == '\0' ||
        settings.mqttUsername[0] == '\0' ||
        settings.mqttPassword[0] == '\0' ||
        settings.mqttHost[0] == '\0' ||
        settings.mqttTopic[0] == '\0'
    ){
        return false;
    }

    if(!isValidPort(settings.mqttPort)){
        return false;
    }

    if(settings.measurementInterval < 100){
        return false;
    }

    return true;
}