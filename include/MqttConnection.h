#pragma once
#include <ESP32MQTTClient.h>
#include "DeviceSettings.h"

class MqttConnection {
    public:
        static std::atomic<bool> sending;

        static ESP32MQTTClient mqttClient;
        
        static void init(const DeviceSettings& settings);
        static void publish(const char* message, const char* topic);
};