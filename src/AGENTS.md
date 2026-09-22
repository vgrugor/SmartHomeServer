# Firmware source guidance

These instructions apply to implementation files under `src/`. Also follow the repository-level `AGENTS.md`.

## Architecture boundaries

- `domain/` owns sensor state and domain abstractions. Avoid ESP8266 networking or presentation concerns here.
- `application/` translates application messages and owns the event contracts used across adapters. Keep JSON field names compatible with `data/script.js` and `data/index.html`.
- `config/` contains tracked device behavior and the ignored local secrets implementation.
- `infrastructure/` wraps hardware, storage, connectivity, WebSocket transport, and OTA.
- `presentation/` owns HTTP endpoints and event observers.
- `main.cpp` should remain a small composition root rather than accumulating business logic.

When adding a class, place its header in the matching path under `include/` and its implementation under `src/`.

## Embedded constraints

- Avoid long blocking work in `loop()` and callbacks. Existing delays in connection notifications are legacy behavior; do not add more without a hardware reason.
- Be conservative with heap allocations and repeated `String` construction on the ESP8266.
- Keep asynchronous callback lifetimes safe. Objects referenced by callbacks currently have static storage duration through globals in `main.cpp`.
- Validate WebSocket frame boundaries and HTTP parameters before reading data.
- Do not change pin modes, active-high/active-low semantics, network configuration, or OTA behavior without documenting the hardware impact.
- Preserve the six `SensorType` values and their dashboard mapping unless the task explicitly changes the protocol.

## Verification

Compile with `pio run` after C++ or configuration changes. If logic is made host-testable, add focused PlatformIO tests under `test/`; do not require live Wi-Fi or a physical board for ordinary unit tests.
