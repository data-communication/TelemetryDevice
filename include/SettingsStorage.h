#pragma once

#include "DeviceSettings.h"

class SettingsStorage{
    public:
        void load(DeviceSettings &settings);
        bool save(const DeviceSettings &settings);
};