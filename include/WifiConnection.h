#pragma once

#include "DeviceSettings.h"

#define RETRY_INTERVAL 10 * 1000

class WifiConnection {    
    static unsigned long lastRetryTime;
    static bool wasConnected;

    public: 
        static void initWifi(const DeviceSettings &setitngs);
        static bool update();
};