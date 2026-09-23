# LittleFS dashboard guidance

These instructions apply to files under `data/`. Also follow the repository-level `AGENTS.md`.

## Deployment model

Everything in this directory is served from ESP8266 LittleFS. Deploy dashboard changes with:

```sh
pio run -t uploadfs
```

Do not run that hardware-affecting command unless the user explicitly asks. Firmware upload and filesystem upload are separate operations.

## Browser/firmware contract

- The WebSocket URL is `/ws` on the current host.
- On connect, the browser sends the exact, case-sensitive command `getValues`.
- Incoming JSON value keys are DOM element IDs: `sliderValue1` through `sliderValue6`; values are strings formatted to two decimal places.
- Each value also has a `sliderValueNAgeMinutes` key containing whole elapsed minutes or `null` when it has never been updated. The dashboard renders these into `sliderValueNAge` elements and advances them locally once per minute.
- Readings that are at least 60 minutes old receive the `is-stale` class on their `.sensor-value` wrapper and are displayed in muted gray until a fresh update arrives.
- The six values represent house temperature, outdoor temperature, shower-water temperature, water volume, battery voltage, and battery percentage in that order.
- Navigation targets use fixed controller IP addresses. Treat changes to them as deployment configuration changes, not cosmetic edits.

When changing IDs, payload shape, routes, or units, update the corresponding C++ transformer/server code in the same change.

## Frontend constraints and checks

- Keep the dashboard usable on narrow/mobile screens and lightweight enough for ESP8266 storage and delivery.
- Do not introduce a build pipeline or external CDN dependency unless explicitly requested; the current assets are static HTML, CSS, JavaScript, PNG, and SVG files.
- Preserve the disconnected-state warning and WebSocket reconnect behavior when editing connection logic.
- Check the browser console for syntax/runtime errors and exercise open, close/reconnect, initial values, and incoming update flows when browser testing is available.
- Run `npm run test:browser` when changing `index.html` or `script.js`. The suite covers the active tab, initial request, all six values, malformed messages, offline status, and reconnect delay.
- Avoid caching changes that could mask a newly uploaded dashboard; the current HTML deliberately disables caching.
