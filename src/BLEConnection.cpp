#include <NimBLEDevice.h>
#include "BLEConnection.h"
#include "MqttConnection.h"

class CommandCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* characteristic, NimBLEConnInfo& connInfo) override {
        auto value = characteristic->getValue();
        std::string command(value.c_str(), value.size());

        BLEConnection::Request request{};

        if (command == "save") {
            request.action = BLEConnection::Action::Save;
            BLEConnection::setResult("saving");
        }
        else if (command == "apply") {
            request.action = BLEConnection::Action::Apply;
            BLEConnection::setResult("applying");
        }
        else if (command == "mac"){
            request.action = BLEConnection::Action::Mac;
            BLEConnection::setResult(WiFi.macAddress().c_str());
            return;
        }
        else {
            BLEConnection::setResult("error:unknown_command");
            return;
        }

        if (xQueueSend(BLEConnection::requests, &request, 0) != pdTRUE) {
            BLEConnection::setResult("error:busy");
        }
    }
};

class SettingsCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* characteristic, NimBLEConnInfo& connInfo) override {

        auto value = characteristic->getValue();

        if (characteristic == BLEConnection::deviceId) {
            BLEConnection::readField(BLEConnection::deviceId, BLEConnection::settingsPtr->deviceId);
            Serial.println(F("Updated deviceId."));
        }

        else if (characteristic == BLEConnection::deviceLocation) {
            BLEConnection::readField(BLEConnection::deviceLocation, BLEConnection::settingsPtr->deviceLocation);
            Serial.println(F("Updated deviceLocation."));
        }

        else if (characteristic == BLEConnection::wifiSSID) {
            BLEConnection::readField(BLEConnection::wifiSSID, BLEConnection::settingsPtr->wifiSSID);
            Serial.println(F("Updated wifiSSID."));
        }

        else if (characteristic == BLEConnection::wifiPassword) {
            BLEConnection::readField(BLEConnection::wifiPassword, BLEConnection::settingsPtr->wifiPassword);
            Serial.println(F("Updated wifiPassword."));
        }

        else if (characteristic == BLEConnection::mqttUsername) {
            BLEConnection::readField(BLEConnection::mqttUsername, BLEConnection::settingsPtr->mqttUsername);
            Serial.println(F("Updated mqttUsername"));
        }

        else if (characteristic == BLEConnection::mqttPassword) {
            BLEConnection::readField(BLEConnection::mqttPassword, BLEConnection::settingsPtr->mqttPassword);
            Serial.println(F("Updated mqttPassword"));
        }

        else if (characteristic == BLEConnection::mqttHost) {
            BLEConnection::readField(BLEConnection::mqttHost, BLEConnection::settingsPtr->mqttHost);
            Serial.println(F("Updated mqttHost"));
        }

        else if (characteristic == BLEConnection::mqttTopic) {
            BLEConnection::readField(BLEConnection::mqttTopic, BLEConnection::settingsPtr->mqttTopic);
            Serial.println(F("Updated mqttTopic"));
        }

        else if (characteristic == BLEConnection::mqttPort) {
            BLEConnection::readField(BLEConnection::mqttPort, BLEConnection::settingsPtr->mqttPort);
            Serial.println(F("Updated mqttPort"));
        }
    }
};

CommandCallbacks commandCallbacks;
SettingsCallbacks settingsCallbacks;

DeviceSettings* BLEConnection::settingsPtr = nullptr;

NimBLECharacteristic* BLEConnection::deviceId = nullptr;
NimBLECharacteristic* BLEConnection::deviceLocation = nullptr;

NimBLECharacteristic* BLEConnection::wifiSSID = nullptr;
NimBLECharacteristic* BLEConnection::wifiPassword = nullptr;

NimBLECharacteristic* BLEConnection::mqttUsername = nullptr;
NimBLECharacteristic* BLEConnection::mqttPassword = nullptr;
NimBLECharacteristic* BLEConnection::mqttHost = nullptr;
NimBLECharacteristic* BLEConnection::mqttTopic = nullptr;
NimBLECharacteristic* BLEConnection::mqttPort = nullptr;

NimBLECharacteristic* BLEConnection::result = nullptr;

QueueHandle_t BLEConnection::requests = nullptr;

bool BLEConnection::savedSuccessfully = false;
bool BLEConnection::restartPending = false;
unsigned long BLEConnection::restartRequestedAt = 0;

void BLEConnection::setResult(const char* message) {
    result->setValue(message);
}

bool BLEConnection::readField(NimBLECharacteristic* characteristic, uint16_t &destination){
    auto value = characteristic->getValue();

    destination = static_cast<uint16_t>(atoi(value.c_str()));

    return true;
}

void BLEConnection::init(DeviceSettings& settings) {
    BLEConnection::settingsPtr = &settings;

    BLEConnection::requests = xQueueCreate(2, sizeof(BLEConnection::Request));

    if (BLEConnection::requests == nullptr) {
        Serial.println(F("Could not create BLE request queue."));
        return;
    }

    NimBLEDevice::init(BLEConnection::settingsPtr->deviceId);

    uint32_t passkey = 100000 + esp_random() % 900000;

    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
    NimBLEDevice::setSecurityPasskey(passkey);

    Serial.print(F("BLE pairing PIN: "));
    Serial.println(passkey);

    auto server = NimBLEDevice::createServer();
    server->advertiseOnDisconnect(true);

    auto service = server->createService(BLEConnection::SERVICE_UUID);

    BLEConnection::deviceId = BLEConnection::createSetting(service, BLEConnection::DEVICE_ID_UUID, BLEConnection::settingsPtr->deviceId);
    BLEConnection::deviceLocation = BLEConnection::createSetting(service, BLEConnection::DEVICE_LOCATION_UUID, BLEConnection::settingsPtr->deviceLocation);
    
    BLEConnection::wifiSSID = BLEConnection::createSetting(service, BLEConnection::WIFI_SSID_UUID, BLEConnection::settingsPtr->wifiSSID);
    BLEConnection::wifiPassword = BLEConnection::createSetting(service, BLEConnection::WIFI_PASSWORD_UUID, BLEConnection::settingsPtr->wifiPassword);

    BLEConnection::mqttUsername = BLEConnection::createSetting(service, BLEConnection::MQTT_USERNAME_UUID, BLEConnection::settingsPtr->mqttUsername);
    BLEConnection::mqttPassword = BLEConnection::createSetting(service, BLEConnection::MQTT_PASSWORD_UUID, BLEConnection::settingsPtr->mqttPassword);
    BLEConnection::mqttHost = BLEConnection::createSetting(service, BLEConnection::MQTT_HOST_UUID, BLEConnection::settingsPtr->mqttHost);
    BLEConnection::mqttTopic = BLEConnection::createSetting(service, BLEConnection::MQTT_TOPIC_UUID, BLEConnection::settingsPtr->mqttTopic);
    BLEConnection::mqttPort = BLEConnection::createSetting(service, BLEConnection::MQTT_PORT_UUID, BLEConnection::settingsPtr->mqttPort);

    auto command = service->createCharacteristic(
        BLEConnection::COMMAND_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_AUTHEN
    );

    command->setCallbacks(&commandCallbacks);
    BLEConnection::deviceId->setCallbacks(&settingsCallbacks);
    BLEConnection::deviceLocation->setCallbacks(&settingsCallbacks);
    
    BLEConnection::wifiSSID->setCallbacks(&settingsCallbacks);
    BLEConnection::wifiPassword->setCallbacks(&settingsCallbacks);

    BLEConnection::mqttUsername->setCallbacks(&settingsCallbacks);
    BLEConnection::mqttPassword->setCallbacks(&settingsCallbacks);
    BLEConnection::mqttHost->setCallbacks(&settingsCallbacks);
    BLEConnection::mqttTopic->setCallbacks(&settingsCallbacks);
    BLEConnection::mqttPort->setCallbacks(&settingsCallbacks);

    BLEConnection::result = service->createCharacteristic(
        BLEConnection::RESULT_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_AUTHEN
    );

    BLEConnection::setResult("ready");

    NimBLEAdvertisementData advertisementData;
    advertisementData.setFlags(0x06);
    advertisementData.addServiceUUID(BLEConnection::SERVICE_UUID);

    NimBLEAdvertisementData scanResponseData;
    scanResponseData.setName(BLEConnection::settingsPtr->deviceId);

    auto advertising = NimBLEDevice::getAdvertising();
    advertising->setAdvertisementData(advertisementData);
    advertising->setScanResponseData(scanResponseData);
    advertising->enableScanResponse(true);
    advertising->start();

    Serial.print(F("BLE available as: "));
    Serial.println(BLEConnection::settingsPtr->deviceId);
}

void BLEConnection::update(SettingsStorage& storage) {
    if (requests == nullptr) {
        return;
    }

    if(restartPending){
        if (millis() - restartRequestedAt >= 1500) {
            NimBLEDevice::getAdvertising()->stop();
            NimBLEDevice::deinit(true);

            esp_bt_controller_disable();
            esp_bt_controller_deinit();

            delay(200);

            esp_restart();
        }
        return;
    }

    BLEConnection::Request request;

    if(xQueueReceive(requests, &request, 0) != pdTRUE){
        return;
    }

    if(request.action == Action::Save){
        setResult("saving");
        savedSuccessfully = storage.save(*BLEConnection::settingsPtr);

        if(savedSuccessfully){
            setResult("saved");
            Serial.println(F("Settings saved. Waiting for apply."));
        }
        else{
            setResult("error:save_failed");
            Serial.println(F("Could not save settings."));
        }
    }
    else if(request.action == Action::Apply){
        if(!savedSuccessfully){
            setResult("error:save_first");
            return;
        }

        setResult("restarting");
        restartRequestedAt = millis();
        restartPending = true;

        Serial.println(F("Restarting with saved settings..."));
    }
}