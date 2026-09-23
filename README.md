# SmartHomeServer

Firmware for a NodeMCU v2 / ESP8266 home controller. The controller receives indoor and outdoor temperature readings and shower-system data, displays them on a web dashboard, and pushes updates to connected browsers over WebSocket.

## Features

- ESP8266 Wi-Fi connection with a static IP address;
- LittleFS-hosted web dashboard;
- sensor updates over HTTP;
- real-time browser synchronization over WebSocket;
- last-update age for every displayed parameter;
- rotating full-screen sensor readings on a 2.8-inch ST7789V TFT;
- LED, buzzer, Serial, and WebSocket event notifications;
- Arduino OTA updates for firmware and LittleFS;
- native unit tests and dashboard browser tests.

## Hardware and software

- NodeMCU v2 / ESP8266;
- Arduino framework;
- PlatformIO;
- LittleFS;
- ESPAsyncWebServer and WebSockets;
- Node.js 20+ and Playwright for browser tests.

## Repository structure

```text
include/                 C++ headers
src/
  application/           Update use cases, validation, and data transformation
  config/                Device configuration and local secrets
  domain/                Sensor state and types
  infrastructure/        Wi-Fi, OTA, LittleFS, WebSocket, and hardware adapters
  presentation/          HTTP routes and event observers
data/                    Static LittleFS dashboard
test/                    PlatformIO native unit tests
tests/browser.cjs        Dashboard browser tests
```

See `AGENTS.md` for detailed architectural contracts and instructions for coding agents.

## Initial setup

1. Install [PlatformIO](https://platformio.org/install) and Node.js 20 or newer.
2. Create the local secrets file:

   ```sh
   cp src/config/Secrets.cpp.example src/config/Secrets.cpp
   ```

3. Set the following values in `src/config/Secrets.cpp`:

   - Wi-Fi name and password;
   - controller IP address, gateway, and subnet;
   - Arduino OTA hostname and password.

`src/config/Secrets.cpp` is ignored by Git. Never commit real credentials or add them to CI or tracked configuration examples.

## Build and test

Build the ESP8266 firmware:

```sh
pio run -e nodemcuv2
```

Run the native unit tests:

```sh
pio test -e native
```

Install the dashboard test dependencies and run the browser tests:

```sh
npm ci
npm run test:browser
```

CI runs all three checks for pushes and pull requests.

## HTTP API

Update routes accept parameters as a query string for `GET` or as form data for `POST`.

| Route | Parameters | Purpose |
| --- | --- | --- |
| `/house/temperature` | `temp` | Indoor temperature |
| `/outdoor/temperature` | `temp` | Outdoor temperature |
| `/shower/update` | `temp`, `liter`, `voltage`, `percent` | Water temperature and volume, battery voltage and charge |

Examples:

```sh
curl --request POST --data 'temp=21.75' http://DEVICE_IP/house/temperature
curl --request POST --data 'temp=8.50' http://DEVICE_IP/outdoor/temperature
curl --request POST \
  --data 'temp=39.25&liter=87.50&voltage=12.45&percent=76.00' \
  http://DEVICE_IP/shower/update
```

Missing or invalid values return HTTP `400`. Sensor state is stored in memory and resets when the controller restarts.

## WebSocket API

The WebSocket endpoint is `ws://DEVICE_IP/ws`. After connecting, send the exact command `getValues` to receive the current state:

```json
{
  "sliderValue1": "21.75",
  "sliderValue2": "8.50",
  "sliderValue3": "39.25",
  "sliderValue4": "87.50",
  "sliderValue5": "12.45",
  "sliderValue6": "76.00",
  "sliderValue1AgeMinutes": 0,
  "sliderValue2AgeMinutes": 12,
  "sliderValue3AgeMinutes": 94,
  "sliderValue4AgeMinutes": 94,
  "sliderValue5AgeMinutes": 94,
  "sliderValue6AgeMinutes": 94
}
```

Sensor values are strings formatted to two decimal places. Each `AgeMinutes` field contains the number of complete minutes since that reading was updated, or `null` if the reading has never been received. The dashboard displays minutes, hours with minutes, or rounded days and advances the displayed age locally once per minute. Readings that have never been updated or are at least 60 minutes old are shown in muted gray until fresh data arrives.

## Uploading to a device

Firmware and dashboard assets are uploaded separately:

```sh
pio run -e nodemcuv2 -t upload
pio run -e nodemcuv2 -t uploadfs
```

For OTA deployment, configure `upload_protocol = espota`, the device IP address, and OTA authentication locally in PlatformIO. Do not commit the password. The password can be read from an environment variable instead of being stored in `platformio.ini`:

```ini
upload_protocol = espota
upload_port = 192.168.1.200
upload_flags =
    --port=8266
    --auth=${sysenv.OTA_PASSWORD}
```

Then provide the password only for the upload process:

```sh
OTA_PASSWORD='your_password' pio run -e nodemcuv2 -t upload
OTA_PASSWORD='your_password' pio run -e nodemcuv2 -t uploadfs
```

Changes under `src/` or `include/` require a firmware upload. Changes under `data/` require a separate LittleFS upload.

## Local ST7789V display

The firmware supports the 2.8-inch 240×320 SPI ST7789V module in landscape orientation. It shows one reading at a time using the largest text that fits and advances every five seconds. Missing values and readings that are at least one hour old are excluded from the rotation and remain available only in the web dashboard. The display shows `NO FRESH DATA` when no current readings are available.

Connect the display as follows:

| Display pin | NodeMCU pin | Purpose |
| --- | --- | --- |
| `SCL` | `D5 / GPIO14` | Hardware SPI clock |
| `SDA` | `D7 / GPIO13` | Hardware SPI MOSI |
| `CS` | `D8 / GPIO15` | Chip select |
| `DC` | `D2 / GPIO4` | Data/command selection |
| `RST` | NodeMCU `RST` | Shared hardware reset |
| `BL` | `3.3V` | Always-on backlight |
| `VCC` | `3.3V` or `5V` | Follow the module PCB marking |
| `GND` | `GND` | Common ground |

To free the hardware SPI pins, the external status LED is connected to `D1 / GPIO5` and the buzzer to `D0 / GPIO16`. Rewire both components before installing this firmware. Do not upload the firmware to hardware that still uses the previous `D6` and `D7` actuator wiring.
