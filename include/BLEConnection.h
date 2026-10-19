#pragma once

#include <NimBLEDevice.h>
#include "DeviceSettings.h"
#include "SettingsStorage.h"

class BLEConnection {
    static bool savedSuccessfully;
    static bool restartPending;
    static unsigned long restartRequestedAt;

    template<typename T>
    static NimBLECharacteristic* createSetting(NimBLEService* service, const char* uuid, const T& value){
        uint32_t properties =
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::READ_AUTHEN |
            NIMBLE_PROPERTY::WRITE_AUTHEN;

        auto characteristic = service->createCharacteristic(uuid, properties);
        characteristic->setValue(String(value).c_str());

        return characteristic;
    }

    public: 
        static DeviceSettings* settingsPtr;

        static QueueHandle_t requests;

        static NimBLECharacteristic* deviceId;
        static NimBLECharacteristic* deviceLocation;
        static NimBLECharacteristic* measurementInterval;

        static NimBLECharacteristic* wifiSSID;
        static NimBLECharacteristic* wifiPassword;

        static NimBLECharacteristic* mqttUsername;
        static NimBLECharacteristic* mqttPassword;
        static NimBLECharacteristic* mqttHost;
        static NimBLECharacteristic* mqttTopic;
        static NimBLECharacteristic* mqttPort;

        static NimBLECharacteristic* result;

        inline static constexpr char SERVICE_UUID[]                 = "9b43d9ad-bbac-4bf2-bfbc-715c00000000";
        
        inline static constexpr char DEVICE_ID_UUID[]               = "9b43d9ad-bbac-4bf2-bfbc-715c00000001";
        inline static constexpr char DEVICE_LOCATION_UUID[]         = "9b43d9ad-bbac-4bf2-bfbc-715c00000002";
        inline static constexpr char MEASUREMENT_INTERVAL_UUID[]    = "9b43d9ad-bbac-4bf2-bfbc-715c00000003";
        
        inline static constexpr char WIFI_SSID_UUID[]               = "9b43d9ad-bbac-4bf2-bfbc-715c00000004";
        inline static constexpr char WIFI_PASSWORD_UUID[]           = "9b43d9ad-bbac-4bf2-bfbc-715c00000005";
        
        inline static constexpr char MQTT_USERNAME_UUID[]           = "9b43d9ad-bbac-4bf2-bfbc-715c00000006";
        inline static constexpr char MQTT_PASSWORD_UUID[]           = "9b43d9ad-bbac-4bf2-bfbc-715c00000007";
        inline static constexpr char MQTT_HOST_UUID[]               = "9b43d9ad-bbac-4bf2-bfbc-715c00000008";
        inline static constexpr char MQTT_TOPIC_UUID[]              = "9b43d9ad-bbac-4bf2-bfbc-715c00000009";
        inline static constexpr char MQTT_PORT_UUID[]               = "9b43d9ad-bbac-4bf2-bfbc-715c00000010";
        
        inline static constexpr char COMMAND_UUID[]                 = "9b43d9ad-bbac-4bf2-bfbc-715c00000011";
        inline static constexpr char RESULT_UUID[]                  = "9b43d9ad-bbac-4bf2-bfbc-715c00000012";

        enum class Action : uint8_t {
            Save,
            Apply,
            Mac
        };

        struct Request {
            Action action;
            DeviceSettings settings;
        };
        
        static void init(DeviceSettings& settings);
        static void update(SettingsStorage& storage);
        static void setResult(const char* message);

        template <size_t N>
        static bool readField(NimBLECharacteristic* characteristic, char (&destination)[N]){
            auto value = characteristic->getValue();

            if (value.size() >= N ||
                memchr(value.data(), '\0', value.size()) != nullptr) {
            return false;
            }

            memset(destination, 0, N);
            memcpy(destination, value.data(), value.size());

            return true;
        }
        static bool readField(NimBLECharacteristic* characteristic, uint16_t& destination);
};