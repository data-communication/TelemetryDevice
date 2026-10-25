#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <time.h>

#include "DeviceSettings.h"
#include "SettingsStorage.h"
#include "WifiConnection.h"
#include "rgb_led.h"
#include "BLEConnection.h"
#include "MqttConnection.h"

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define SETTINGS_PRINT_INTERVAL 5000

DHT dht(DHT_PIN, DHT_TYPE);

DeviceSettings settings;
SettingsStorage storage;

void printSettings(){
    Serial.print("DeviceId: ");
    Serial.println(settings.deviceId);

    Serial.print("DeviceLocation: ");
    Serial.println(settings.deviceLocation);
    
    Serial.print("MeasurementInterval: ");
    Serial.println(settings.measurementInterval);

    Serial.print("WifiSSID: ");
    Serial.println(settings.wifiSSID);

    Serial.print("WifiPassword: ");
    Serial.println(settings.wifiPassword);

    Serial.print("Mqtt Username: ");
    Serial.println(settings.mqttUsername);

    Serial.print("Mqtt Password: ");
    Serial.println(settings.mqttPassword);

    Serial.print("Mqtt Host: ");
    Serial.println(settings.mqttHost);

    Serial.print("Mqtt Topic: ");
    Serial.println(settings.mqttTopic);

    Serial.print("Mqtt Port: ");
    Serial.println(settings.mqttPort);
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();

    rgb_init();

    storage.load(settings);
    if (settings.deviceId[0] == '\0'){
        strcpy(settings.deviceId, WiFi.macAddress().c_str());
        storage.save(settings);
    }

    printSettings();

    BLEConnection::init(settings);
    WifiConnection::init(settings);
    MqttConnection::init(settings);

    dht.begin();
}

void loop() {
    BLEConnection::update(storage);

    static unsigned long lastSettingsPrint = 0;
    if(!validSettings(settings)){
        if(millis() - lastSettingsPrint >= SETTINGS_PRINT_INTERVAL){
            printSettings();

            Serial.print(F("------------------"));
            Serial.print(F("Invalid Settings."));
            Serial.println(F("------------------"));

            lastSettingsPrint = millis();
        }
        rgb_yellow();
        return;
    }

    if(!WifiConnection::update()){
        return;
    }

    static unsigned long lastMeasurement = 0;
    if(millis() - lastMeasurement >= settings.measurementInterval){
        float humi = dht.readHumidity();
        float temp = dht.readTemperature();

        if(isnan(humi) || isnan(temp)){
            Serial.println(F("Failed to read DHT11 sensor."));
            return;
        }

        time_t now = time(nullptr);
        long utc = static_cast<long>(now);

        Serial.println();
        Serial.print(F("Humidity: "));
        Serial.print(humi);
        Serial.println(F("%"));
        Serial.print(F("Temperature: "));
        Serial.print(temp);
        Serial.println(F("°C"));
        Serial.print(F("Time: "));
        Serial.println(utc);

        char payload[256];
        snprintf(payload, 
            sizeof(payload), 
            "{\"deviceId\":\"%s\""
            ",\"measurements\":["
                "{\"type\":\"temperature\",\"value\":%.2f,\"unit\":\"°C\"},"
                "{\"type\":\"humidity\",\"value\":%.2f,\"unit\":\"%%\"}],"
            "\"timestamp\":\"%ld\"}",
            settings.deviceId,
            temp,
            humi,
            utc
        );

        MqttConnection::publish(payload, settings.mqttTopic);
        Serial.println(F("--------------------"));
        lastMeasurement = millis();
    }
}