#pragma once
#include <Arduino.h>

typedef struct {
    String deviceId;
    String deviceLocation;
    
    String wifiSSID;
    String wifiPassword;
} DeviceSettings;