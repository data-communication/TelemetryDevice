# TelemetryDevice

TelemetryDevice is firmware for an Arduino Nano ESP32 that reads temperature and humidity from a DHT11 sensor and publishes the measurements to an MQTT broker. Wi-Fi and MQTT settings are provisioned over Bluetooth Low Energy (BLE) and stored in the device's non-volatile preferences.

## Hardware

- Arduino Nano ESP32
- DHT11 temperature and humidity sensor connected to GPIO 4

The board's built-in RGB LED indicates device state:

- Yellow: settings are missing or invalid
- Green: connected to Wi-Fi
- Red, then blinking blue: Wi-Fi disconnected and reconnecting

## Build and upload

This project uses PlatformIO. Install [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) or the PlatformIO IDE extension, connect the board over USB, then run:

```sh
pio run
pio run --target upload
pio device monitor --baud 115200
```

The PlatformIO environment is `arduino_nano_esp32`. Dependencies are declared in [`platformio.ini`](platformio.ini) and are installed by PlatformIO.

## First-time setup

1. Build and upload the firmware, then open the serial monitor at **115200 baud**.
2. Find the BLE device. The device advertises a custom service UUID and uses its device ID as its BLE name. If no device ID has been saved, firmware fills it with the board's Wi-Fi MAC address at startup and attempts to save it.
3. Pair using the passkey printed in the serial monitor at startup.
4. Connect with a BLE client that supports authenticated GATT reads and writes. Write the settings listed below to their corresponding characteristics.
5. Write `save` to the command characteristic and check the result characteristic for `saved`.
6. Write `apply` to restart the board with the saved settings.

The BLE service UUID is `9b43d9ad-bbac-4bf2-bfbc-715c00000000`. Settings and command/result characteristics require authenticated BLE access.

| Setting | Characteristic UUID | Accepted value | Maximum length |
|---|---|---|---:|
| Device ID | `9b43d9ad-bbac-4bf2-bfbc-715c00000001` | Non-empty identifier; defaults to the Wi-Fi MAC address on startup if empty | 36 characters |
| Device location | `9b43d9ad-bbac-4bf2-bfbc-715c00000002` | Non-empty location | 32 characters |
| Measurement interval | `9b43d9ad-bbac-4bf2-bfbc-715c00000003` | Decimal milliseconds | 2 bytes (`uint16_t`); 100–65,535 ms |
| Wi-Fi SSID | `9b43d9ad-bbac-4bf2-bfbc-715c00000004` | Network name | 32 characters |
| Wi-Fi password | `9b43d9ad-bbac-4bf2-bfbc-715c00000005` | Network password | 64 characters |
| MQTT username | `9b43d9ad-bbac-4bf2-bfbc-715c00000006` | Broker username | 32 characters |
| MQTT password | `9b43d9ad-bbac-4bf2-bfbc-715c00000007` | Broker password | 64 characters |
| MQTT host | `9b43d9ad-bbac-4bf2-bfbc-715c00000008` | Broker hostname or address | 32 characters |
| MQTT topic | `9b43d9ad-bbac-4bf2-bfbc-715c00000009` | Publish topic | 95 characters |
| MQTT port | `9b43d9ad-bbac-4bf2-bfbc-715c00000010` | `1883` or `8883` | — |
| Command | `9b43d9ad-bbac-4bf2-bfbc-715c00000011` | `save`, `apply`, or `mac` | — |
| Result | `9b43d9ad-bbac-4bf2-bfbc-715c00000012` | Read operation status/result | — |

Maximum lengths apply to text values; the storage buffers also reserve one byte for the null terminator. Write numeric settings as decimal text.

The `mac` command returns the board's Wi-Fi MAC address in the result characteristic. A command result may report errors such as `error:save_first`, `error:save_failed`, `error:busy`, or `error:unknown_command`.

## MQTT messages

Once settings are valid and Wi-Fi is connected, the device reads the DHT11 at the configured interval and publishes JSON to the configured topic. The timestamp is UTC Unix time synchronized using NTP (`pool.ntp.org` and `time.nist.gov`).

Example:

```json
{
  "deviceId": "sensor-01",
  "measurements": [
    { "type": "temperature", "value": 21.50, "unit": "°C" },
    { "type": "humidity", "value": 45.00, "unit": "%" }
  ],
  "timestamp": "1761379200"
}
```

## Project layout

- `src/main.cpp`: startup, sensor sampling, and telemetry publishing
- `src/BLEConnection.cpp`: BLE provisioning and commands
- `src/WifiConnection.cpp`: Wi-Fi connection and retry handling
- `src/MqttConnection.cpp`: MQTT client setup and publishing
- `src/SettingsStorage.cpp`: persistent settings storage
- `src/rgb_led.cpp`: status LED
- `include/`: project headers

## Notes

- The serial startup log currently prints the device settings, including Wi-Fi and MQTT passwords. Treat serial logs and monitor output as sensitive.
- The firmware accepts MQTT port values `1883` and `8883`; configure the broker and network accordingly.
- PlatformIO's `test/` directory is present, but no project-specific tests are currently defined.
