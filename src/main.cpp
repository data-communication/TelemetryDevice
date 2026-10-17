#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>

#include "DeviceSettings.h"
#include "SettingsStorage.h"
#include "WifiConnection.h"

#define DHT_PIN 4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

DeviceSettings settings;
SettingsStorage storage;

void setup() {
    Serial.begin(115200);
    delay(2000);

    storage.load(settings);

    WifiConnection::initWifi(settings);
    
    dht.begin();
}

void loop() {
    delay(5000);
    WifiConnection::update();

    Serial.print(F("Device Id: "));
    Serial.println(settings.deviceId);

    float humi = dht.readHumidity();
    float temp = dht.readTemperature();

    if(isnan(humi) || isnan(temp)){
        Serial.println(F("Failed to read DHT11 sensor."));
        return;
    }

    Serial.println(F("--------------------"));
    Serial.print(F("Humidity: "));
    Serial.print(humi);
    Serial.println(F("%"));
    Serial.print(F("Temperature: "));
    Serial.print(temp);
    Serial.println(F("°C"));
}