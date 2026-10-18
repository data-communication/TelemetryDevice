#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>

#include "DeviceSettings.h"
#include "SettingsStorage.h"
#include "WifiConnection.h"
#include "rgb_led.h"
#include "BLEConnection.h"

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define MEASUREMENT_TIMER 5 * 1000

DHT dht(DHT_PIN, DHT_TYPE);

DeviceSettings settings;
SettingsStorage storage;

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();

    rgb_init();

    storage.load(settings);

    WifiConnection::initWifi(settings);
    BLEConnection::init(settings);

    dht.begin();
}

void loop() {
    BLEConnection::update(storage);

    if(!WifiConnection::update()){
        return;
    }
    
    static unsigned long lastMeasurement = 0;
    if(millis() - lastMeasurement >= MEASUREMENT_TIMER){
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

        lastMeasurement = millis();
    }   
}