# SmartHomeServer

Firmware for a NodeMCU v2 / ESP8266 home controller. The controller receives indoor and outdoor temperature readings and shower-system data, displays them on a web dashboard, and pushes updates to connected browsers over WebSocket.

## Features

- ESP8266 Wi-Fi connection with a static IP address;
- DNS via the configured gateway for NTP and Telegram hostname resolution;
- LittleFS-hosted web dashboard;
- sensor updates over HTTP;
- real-time browser synchronization over WebSocket;
- last-update age for every displayed parameter;
- LED, buzzer, Serial, and WebSocket event notifications;
- Arduino OTA updates for firmware and LittleFS;
- a daily indoor/outdoor temperature report through a Telegram bot;
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
   - Telegram bot token and destination chat ID for the daily report.

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

## Daily Telegram report

Set `TELEGRAM_BOT_TOKEN` and `TELEGRAM_CHAT_ID` in the ignored `src/config/Secrets.cpp`. Start a conversation with the bot (or add it to the destination group) so it can send messages there. Do not put the token in tracked files or CI.

At or after 15:00 Europe/Kyiv each day, the controller sends one message containing the current indoor and outdoor temperatures and the actual local send time. It waits for Wi-Fi, synchronized time, and both temperature readings. If power is off at 15:00, it sends after the controller restarts and receives both readings. Failed requests retry every five minutes. A confirmed delivery date is stored in ESP8266 EEPROM emulation, so an ordinary reboot or LittleFS upload does not reset the daily marker. If the marker is unreadable, reports are disabled to avoid accidental duplicates.

The bot connection uses HTTPS certificate validation. Its trust anchor is in `include/infrastructure/reporting/TelegramRootCa.h`; if Telegram changes certificate authorities, the anchor may need updating. The HTTPS request can briefly delay other `loop()` work while a send is in progress.

Telegram's `sendMessage` API does not provide a caller-supplied idempotency key. A power loss after Telegram accepts a message but before the EEPROM commit, or an ambiguous network timeout after acceptance, can therefore still cause a duplicate. The firmware prevents repeats after a confirmed, persisted send, but cannot guarantee exactly-once delivery across those failure windows.

`GET /telegram/report/status` returns `lastSentDate` (a `YYYYMMDD` integer, or `0` if none), storage readiness, local-time validity, reading availability, last send attempt result, and a transport status code. It exposes no bot credentials or chat ID and can be used to verify that a confirmed delivery was persisted. A negative transport code indicates a local connection/setup error; HTTP `200` plus `lastAttemptSucceeded: true` indicates Telegram accepted the message.

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
