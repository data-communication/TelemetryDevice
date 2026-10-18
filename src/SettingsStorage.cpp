#include "SettingsStorage.h"
#include <Preferences.h>
#include <WiFi.h>

void SettingsStorage::load(DeviceSettings &settings){
    Preferences preferences;
    preferences.begin("device-config", true);

    String deviceId = preferences.getString("deviceId", WiFi.macAddress());
    deviceId.toCharArray(settings.deviceId, sizeof(settings.deviceId));

    String deviceLocation = preferences.getString("deviceLocation", "");
    deviceLocation.toCharArray(settings.deviceLocation, sizeof(settings.deviceLocation));

    String wifiSSID = preferences.getString("wifiSSID", "");
    wifiSSID.toCharArray(settings.wifiSSID, sizeof(settings.wifiSSID));

    String wifiPassword = preferences.getString("wifiPassword", "");
    wifiPassword.toCharArray(settings.wifiPassword, sizeof(settings.wifiPassword));

    preferences.end();
}

bool SettingsStorage::save(const DeviceSettings &settings){
    Preferences preferences;
    preferences.begin("device-config", false);

    preferences.putString("deviceId", settings.deviceId);
    preferences.putString("deviceLocation", settings.deviceLocation);
    preferences.putString("wifiSSID", settings.wifiSSID);
    preferences.putString("wifiPassword", settings.wifiPassword);

    preferences.end();

    return true;
}