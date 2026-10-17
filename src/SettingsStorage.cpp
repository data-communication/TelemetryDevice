#include "SettingsStorage.h"
#include <Preferences.h>
#include <WiFi.h>

void SettingsStorage::load(DeviceSettings &settings){
    Preferences preferences;
    preferences.begin("device-config", true);

    settings.deviceId = preferences.getString("deviceId", WiFi.macAddress());
    settings.deviceLocation = preferences.getString("deviceLocation", "");
    settings.wifiSSID = preferences.getString("wifiSSID", "Telia-E70367");
    settings.wifiPassword = preferences.getString("wifiPassword", "pufhskraK6ttPH6Y");

    preferences.end();
}

void SettingsStorage::save(const DeviceSettings &settings){
    Preferences preferences;
    preferences.begin("device-config", false);

    preferences.putString("deviceId", settings.deviceId);
    preferences.putString("deviceLocation", settings.deviceLocation);
    preferences.putString("wifiSSID", settings.wifiSSID);
    preferences.putString("wifiPassword", settings.wifiPassword);

    preferences.end();
}