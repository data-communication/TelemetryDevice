#pragma once
#include <Arduino.h>
#include <WiFi.h>

typedef struct {
    char deviceId[37];
    char deviceLocation[33];
    
    char wifiSSID[33];
    char wifiPassword[65];
} DeviceSettings;