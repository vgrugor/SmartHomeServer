# SmartHomeServer agent guide

## Project purpose

This repository contains firmware for a NodeMCU v2 / ESP8266 smart-home server. It:

- connects to Wi-Fi with a static address;
- serves a dashboard from LittleFS;
- accepts sensor updates over HTTP;
- pushes the current sensor state to browsers over WebSocket;
- reports lifecycle events through LED, buzzer, serial, and WebSocket observers;
- supports Arduino OTA updates.

The project is built with PlatformIO. The `nodemcuv2` environment builds Arduino firmware and the `native` environment runs host-side unit tests.

## Repository map

- `src/main.cpp` is the composition root and contains Arduino `setup()` / `loop()`.
- `include/domain/` and `src/domain/` hold device-independent data types and state.
- `include/application/` and `src/application/` hold sensor update use cases, validation, WebSocket transformations, and application event contracts.
- `include/config/` and `src/config/` separate tracked device configuration from local secrets.
- `include/infrastructure/` and `src/infrastructure/` contain ESP8266, Wi-Fi, LittleFS, OTA, WebSocket, and actuator adapters.
- `include/presentation/` and `src/presentation/` contain HTTP routes and event observers.
- `data/` is the LittleFS web application deployed separately from firmware.
- `test/` contains native tests for device-independent domain and validation logic.

## Local configuration and secrets

`src/config/Secrets.cpp` is intentionally ignored by Git and must remain untracked because it contains Wi-Fi and OTA secrets. Never print, commit, or replace an existing copy.

For a fresh checkout, copy `src/config/Secrets.cpp.example` to `src/config/Secrets.cpp` and fill in the local values. Keep GPIO assignments and HTTP parameter names in the tracked `DeviceConfig` files. Use placeholder values only in tracked examples.

## Common commands

Run commands from the repository root:

```sh
pio run
pio test -e native
pio run -t uploadfs
pio run -t upload
pio device monitor -b 115200
pio run -t clean
```

`pio run` is the normal compile check. `uploadfs`, `upload`, the serial monitor, and OTA/USB device access interact with hardware; run them only when the user explicitly requests device-side work and provides the required connection details.

If `pio` is unavailable, report that verification limitation rather than installing tools or claiming the build passed. The VS Code recommendation is `platformio.platformio-ide`.

## Change and verification rules

- Preserve the current layered directory structure and keep declarations in `include/` paired with implementations in `src/`.
- Use repository-relative includes such as `"presentation/WebServer.h"`.
- Keep credentials, local IP choices, and OTA passwords out of tracked files.
- Changes under `data/` require a LittleFS upload (`pio run -t uploadfs`) to reach a device; a firmware upload alone does not deploy them.
- Changes under `src/`, `include/`, or `platformio.ini` require at least `pio run` when PlatformIO is available.
- Run `pio test -e native` for device-independent logic. Add focused tests under `test/` when more Arduino-free behavior is introduced.
- Do not perform a hardware upload as verification unless explicitly authorized. A successful compile is not evidence that Wi-Fi, HTTP, WebSocket, pins, or OTA work on the physical device.
- Keep changes focused. Do not silently refactor hardware initialization, change GPIO assignments, network addresses, endpoint parameter names, or browser-visible units.

## Runtime flow and contracts

1. Global objects are wired in `src/main.cpp`.
2. `setup()` initializes hardware/filesystem components, registers observers, and starts Wi-Fi connection asynchronously.
3. `loop()` advances the non-blocking Wi-Fi state and services OTA after the network services have started. The HTTP server is asynchronous and needs no polling call.
4. HTTP handlers parse parameters and delegate validation and state changes to `SensorUpdateService`.
5. Each successful operation emits saved-value events and exactly one `WEB_SOCKET_NOTIFY_CLIENT` event.
6. `WsDataTransformer` serializes all six dashboard values as two-decimal strings using the DOM IDs `sliderValue1` through `sliderValue6`.
7. The dashboard connects to `/ws`, sends the exact command `getValues`, and updates elements whose IDs match JSON keys.

Public HTTP routes currently include:

- `GET /`
- `HEAD /`
- `GET /house/temperature`
- `POST /house/temperature`
- `GET /outdoor/temperature`
- `POST /outdoor/temperature`
- `GET /shower/update`
- `POST /shower/update`
- WebSocket `/ws`

The query-parameter names are configuration constants declared in `include/config/DeviceConfig.h`. When changing a route or JSON key, update the C++ handler/transformer and the browser code together.

## Known baseline limitations

- `src/config/Secrets.cpp` is absent in a clean checkout, so firmware does not link until local secrets are created from the example.
- Sensor state is in memory only and returns `0.0` for unset values.

Treat these as existing conditions, not as regressions introduced by unrelated work. Fix them only when they are in scope, and add verification for any behavior you change.
