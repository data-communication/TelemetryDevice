#include <ESP32MQTTClient.h>
#include "MqttConnection.h"

void onMqttConnect(esp_mqtt_client_handle_t client) {}

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_err_t handleMQTT(esp_mqtt_event_handle_t event) {
        MqttConnection::mqttClient.onEventCallback(event);
        return ESP_OK;
    }
#else
    void handleMQTT(void *handlerArgs, esp_event_base_t base, int32_t eventId, void *eventData) {
        auto event = static_cast<esp_mqtt_event_handle_t>(eventData);
        MqttConnection::mqttClient.onEventCallback(event);
    }
#endif

std::atomic<bool> MqttConnection::sending{true};
ESP32MQTTClient MqttConnection::mqttClient{};

void MqttConnection::init(const DeviceSettings& settings) {
    MqttConnection::mqttClient.setURL(
        settings.mqttHost, 
        settings.mqttPort, 
        settings.mqttUsername, settings.mqttPassword
    );
    MqttConnection::mqttClient.setMqttClientName(settings.mqttUsername);
    MqttConnection::mqttClient.loopStart();

    Serial.println(F("MQTT Client running."));
}

void MqttConnection::publish(const char* message, const char* topic) {
    if (!sending.load() || !mqttClient.isConnected())
        return;

    mqttClient.publish(topic, message);
    Serial.println(F("Mqtt Sent."));
}