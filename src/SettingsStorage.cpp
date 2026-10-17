#include "SettingsStorage.h"
#include <Preferences.h>
#include <WiFi.h>

void SettingsStorage::load(DeviceSettings &settings){
    Preferences preferences;
    preferences.begin("device-config", true);

    settings.deviceId = preferences.getString("deviceId", WiFi.macAddress());
    settings.deviceLocation = preferences.getString("deviceLocation", "");
    settings.wifiSSID = preferences.getString("wifiSSID", "");
    settings.wifiPassword = preferences.getString("wifiPassword", "");

    preferences.end();
}

void SettingsStorage::save(const DeviceSettings &settings){
    Preferences preferences;
    preferences.begin("device-config", false);

    preferences.putString("deviceId", settings.deviceId);
    preferences.putString("deviceLocation", settings.deviceLocation);
    preferences.putString("wifiSSID", "");
    preferences.putString("wifiPassword", "");

    preferences.end();
}