# ESP32 RC Car — Firmware (Milestone 1)

Wi-Fi controlled 2-wheel RC car. This milestone covers only the ESP32 firmware —
no Android app yet, per the project plan.

## What this does

- ESP32 boots with motors OFF, starts its own Wi-Fi Access Point (no internet, no router).
- Runs an HTTP server with `/status`, `/move`, `/stop`.
- Drives the L298N via PWM, with a differential-drive core (`driveDifferential(left, right)`)
  so proportional joystick control later is a small addition, not a rewrite.
- Auto-stops the motors if commands stop arriving (failsafe).

## Wi-Fi

| | |
|---|---|
| SSID | `RC-Car` |
| Password | `rccar1234` |
| ESP32 IP | `192.168.4.1` |

Connect your phone to this network directly (Settings → Wi-Fi). No internet on this network — that's expected.

## HTTP API

**GET `/status`**
```json
{ "status": "ok", "connected": true, "direction": "stop", "speed": 0 }
```

**POST `/move`**
```json
{ "direction": "forward", "speed": 180 }
```
`direction`: `forward` | `backward` | `left` | `right` | `stop`
`speed`: `0`–`255` (clamped to `SPEED_DEFAULT_MAX = 200` for now — raise in `config.h` once basic driving is confirmed safe)

Invalid direction, missing field, or bad JSON → `400` with `{"status":"error","message":"..."}`.

**POST `/stop`**
Immediately stops both motors, independent of the failsafe timer.

## GPIO map

| L298N pin | ESP32 GPIO | Role |
|---|---|---|
| ENA | 25 | Left motor speed (PWM) |
| IN1 | 26 | Left motor direction |
| IN2 | 27 | Left motor direction |
| IN3 | 14 | Right motor direction |
| IN4 | 13 | Right motor direction |
| ENB | 33 | Right motor speed (PWM) |
| GND | GND | Shared ground — **required**, see wiring note below |

## Failsafe

If the car is moving and no valid `/move` command arrives for `COMMAND_TIMEOUT_MS`
(default 800 ms, in `config.h`), motors stop automatically and the ESP32 prints:
```
FAILSAFE: COMMAND TIMEOUT
MOTORS STOPPED
```

## Build & flash

This is a PlatformIO project.

```
pio run                    # build
pio run -t upload          # flash over USB
pio device monitor -b 115200   # serial monitor
```

If you're on Arduino IDE instead: create a sketch folder, copy everything from
`src/` into it (rename `main.cpp` → `<foldername>.ino`), install "ArduinoJson"
by Benoit Blanchon (v7) via Library Manager, select an ESP32 dev board, and
upload as usual.

### A note on the ESP32 Arduino core version

This firmware uses the **current (core 3.x)** LEDC PWM API:
`ledcAttach(pin, freq, resolution)` + `ledcWrite(pin, duty)`. Older core 2.x
uses a different, channel-based API (`ledcSetup` + `ledcAttachPin` +
`ledcWrite(channel, duty)`) and will **not** compile this code as-is. If your
installed ESP32 board package is 2.x, update it via Boards Manager /
`pio pkg update` first.

## Wiring safety (read before connecting anything)

- **Power off** while wiring. Do not connect the 4×AA pack until wiring is checked.
- ESP32 stays on USB power. Do **not** connect the AA pack to the ESP32's 3.3V or 5V pin.
- **ESP32 GND and L298N GND must be tied together** — without a common ground,
  the direction/PWM signals have no reliable reference and behavior will be erratic.
- Never wire ESP32 GPIO directly to a motor — the L298N must sit in between.
- Before connecting the battery: verify your specific L298N module's 5V-logic
  jumper/regulator setup against its actual silkscreen labels — modules vary.

## Current status / what's verified

- Firmware logic (Wi-Fi AP setup, HTTP routing, JSON parsing/validation, motor
  control, failsafe) was compiled clean against a real ESP32 toolchain during
  development — 0 errors, 762,001 bytes flash / 44,564 bytes RAM used on a
  generic ESP32 WROOM target.
- The PWM calls specifically (`ledcAttach`/`ledcWrite`) are written to the
  current core 3.x API per Espressif's official docs, but weren't part of that
  same compile pass (environment constraint, not a code concern) — this is the
  one thing worth double-checking output on when you first build it locally.
- Not yet done: wiring to the actual L298N/motors, and the Android app (per
  the project plan, that's Phase 8, after this firmware and the wiring are
  confirmed working).

## Next physical step

1. Build and flash this firmware (commands above).
2. Open the serial monitor — confirm you see AP started / IP address / HTTP
   server started, with motors reported OFF.
3. Connect a phone to the `RC-Car` network and hit `http://192.168.4.1/status`
   in a browser to confirm the API responds — **before** touching the L298N or
   battery.
4. Only once that's confirmed, move on to wiring the L298N (Phase 6/7 in the
   project plan), with power off until wiring is verified.
