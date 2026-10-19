#include "SettingsStorage.h"
#include <Preferences.h>
#include <WiFi.h>

void SettingsStorage::load(DeviceSettings &settings){
    Preferences preferences;
    if (!preferences.begin("device-config", true)) {
        Serial.println("No settings found.");
        return;
    }

    bool loaded =
        preferences.getBytesLength("settings") == sizeof(settings) &&
        preferences.getBytes("settings", &settings, sizeof(settings)) == sizeof(settings);

    preferences.end();
}

bool SettingsStorage::save(const DeviceSettings &settings){
    Preferences preferences;

    if (!preferences.begin("device-config", false)) {
      return false;
    }

    size_t written = preferences.putBytes("settings", &settings, sizeof(settings));
    preferences.end();

    return written == sizeof(settings);
}