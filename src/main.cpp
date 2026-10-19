#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>

#include "DeviceSettings.h"
#include "SettingsStorage.h"
#include "WifiConnection.h"
#include "rgb_led.h"
#include "BLEConnection.h"
#include "MqttConnection.h"

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define MEASUREMENT_TIMER 5 * 1000

DHT dht(DHT_PIN, DHT_TYPE);

DeviceSettings settings;
SettingsStorage storage;

void printSettings(){
    Serial.print("DeviceId: ");
    Serial.println(settings.deviceId);

    Serial.print("DeviceLocation: ");
    Serial.println(settings.deviceLocation);

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
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();

    rgb_init();

    storage.load(settings);

    printSettings();

    WifiConnection::init(settings);
    BLEConnection::init(settings);
    MqttConnection::init(settings);

    dht.begin();
}

void loop() {
    BLEConnection::update(storage);

    static unsigned long lastMeasurement = 0;
    if(millis() - lastMeasurement >= MEASUREMENT_TIMER){
        lastMeasurement = millis();

        if(!validSettings(settings)){
            printSettings();
            Serial.print("------------------");
            Serial.print("Invalid Settings.");
            Serial.println("------------------");
            return;
        }

        if(!WifiConnection::update()){
            return;
        }

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

        char payload[256];
        snprintf(payload, 
            sizeof(payload), 
            "{\"deviceId\":\"%s\""
            ",\"measurements\":["
            "{\"type\":\"temperature\",\"value\":%.2f,\"unit\":\"°C\"},"
            "{\"type\":\"humidity\",\"value\":%.2f,\"unit\":\"%%\"}]}",
            settings.deviceId,
            temp,
            humi
        );
        
        MqttConnection::publish(payload, "test/climate");
    }   
}