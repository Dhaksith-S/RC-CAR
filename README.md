# ESP32 Wi-Fi RC Car

[![Platform](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/Framework-Arduino%20(Core%203.x)-teal.svg)](https://github.com/espressif/arduino-esp32)
[![Build Tool](https://img.shields.io/badge/Build-PlatformIO-orange.svg)](https://platformio.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

A high-performance, Wi-Fi controlled differential-drive RC car powered by an **ESP32** and an **L298N** dual H-bridge motor driver. 

The car creates its own standalone Wi-Fi Access Point (SoftAP) and serves a lightweight RESTful JSON HTTP API with full CORS support. It can be controlled from any web browser, custom mobile app (Android/iOS), gamepad controller, or terminal script without requiring an external Wi-Fi router or internet access.

---

## Key Features

- 📶 **Standalone Wi-Fi Access Point (SoftAP)**: Broadcasts its own secure network (`RC-Car`). Connect directly from any phone, tablet, or laptop.
- 🌐 **RESTful JSON HTTP API**: Control movement with clean JSON payloads over standard HTTP (`/status`, `/move`, `/stop`).
- 🔄 **Full CORS Support**: Preflight `OPTIONS` headers enabled for seamless integration with browser-based web applications and dashboards.
- 🚗 **Differential Drive Steering**: Independent left and right motor speed control with forward, backward, pivot-left, pivot-right, and brake maneuvers.
- ⚡ **Hardware PWM (Core 3.x LEDC)**: Smooth, 8-bit resolution motor speed scaling (0–255 duty cycle) using the latest ESP32 Arduino Core 3.x pin-based PWM API.
- 🛡️ **Built-in Failsafe Protection**: Auto-stops motors if no valid command is received within a configurable timeout window (default 800 ms), preventing runaway cars during Wi-Fi drops or app crashes.

---

## Hardware Architecture & Wiring

### Components
- **Microcontroller**: ESP32 Development Board (ESP32-WROOM-32 / 30-pin or 38-pin)
- **Motor Driver**: L298N Dual H-Bridge Module
- **Motors**: 2× DC Gear Motors (TT Motors) + Rubber Wheels
- **Power Supply**: 
  - ESP32: 5V via USB or regulated step-down
  - Motors: 4× AA battery pack or 2× 18650 Li-ion batteries connected to L298N 12V/VCC input
- **Chassis**: 2WD robot chassis kit with front caster wheel

### GPIO Pinout Mapping

| L298N Pin | ESP32 GPIO | Function / Signal | Notes |
|:---|:---|:---|:---|
| **ENA** | `GPIO 25` | Left Motor Speed (PWM) | 5 kHz, 8-bit LEDC PWM |
| **IN1** | `GPIO 26` | Left Motor Direction A | Digital Output |
| **IN2** | `GPIO 27` | Left Motor Direction B | Digital Output |
| **IN3** | `GPIO 14` | Right Motor Direction A | Digital Output |
| **IN4** | `GPIO 13` | Right Motor Direction B | Digital Output |
| **ENB** | `GPIO 33` | Right Motor Speed (PWM) | 5 kHz, 8-bit LEDC PWM |
| **GND** | `GND` | Common Ground | **Mandatory**: ESP32 GND and L298N GND must be connected together |

> [!IMPORTANT]
> **Common Ground**: Always ensure the ESP32's `GND` is connected to the L298N's `GND`. Without a common ground reference, logic signals will float and cause erratic motor behavior.
>
> **Power Isolation**: Do not attempt to power the DC motors directly from the ESP32 pins or its 3.3V/5V rails. Use an external battery pack for the L298N motor power terminal.

---

## Network & Wi-Fi Details

When powered on, the ESP32 initializes as an Access Point with the following default configuration:

| Setting | Value |
|:---|:---|
| **SSID** | `RC-Car` |
| **Password** | `rccar1234` |
| **IP Address** | `192.168.4.1` |
| **Subnet** | `255.255.255.0` |
| **HTTP Port** | `80` |

---

## HTTP REST API Reference

All requests and responses use standard `application/json`.

### 1. Get Status
Check system health, current direction, and active motor speed.

- **URL**: `/status`
- **Method**: `GET`
- **Response**: `200 OK`
```json
{
  "status": "ok",
  "connected": true,
  "direction": "stop",
  "speed": 0
}
```

---

### 2. Move Car
Send motion commands with direction and speed parameters.

- **URL**: `/move`
- **Method**: `POST`
- **Headers**: `Content-Type: application/json`
- **Request Body**:
```json
{
  "direction": "forward",
  "speed": 180
}
```
- **Allowed Directions**:
  - `"forward"`: Both motors drive forward.
  - `"backward"`: Both motors drive in reverse.
  - `"left"`: Right motor drives forward, left motor idle (pivot turn).
  - `"right"`: Left motor drives forward, right motor idle (pivot turn).
  - `"stop"`: Both motors stopped.
- **Speed Range**: `0` to `255` (default safety cap: `200`, customizable in `config.h`).
- **Response**: `200 OK`
```json
{
  "status": "ok",
  "direction": "forward",
  "speed": 180
}
```

---

### 3. Immediate Emergency Stop
Immediately halts both motors and resets failsafe tracking.

- **URL**: `/stop`
- **Method**: `POST`
- **Response**: `200 OK`
```json
{
  "status": "ok",
  "direction": "stop",
  "speed": 0
}
```

---

### Example cURL Commands

```bash
# Check status
curl http://192.168.4.1/status

# Drive forward at speed 180
curl -X POST http://192.168.4.1/move \
  -H "Content-Type: application/json" \
  -d '{"direction":"forward","speed":180}'

# Pivot left
curl -X POST http://192.168.4.1/move \
  -H "Content-Type: application/json" \
  -d '{"direction":"left","speed":160}'

# Stop immediately
curl -X POST http://192.168.4.1/stop
```

---

## Codebase Structure

```
├── platformio.ini       # PlatformIO configuration & dependencies (ArduinoJson v7)
├── .gitignore           # Git ignore rules for build artifacts (.pio) & editor caches
├── README.md            # Comprehensive project documentation
└── src
    ├── config.h         # Pinouts, Wi-Fi credentials, PWM limits & timeout settings
    ├── main.cpp         # Setup initialization and main execution loop
    ├── wifi_control.h   # Wi-Fi SoftAP interface
    ├── wifi_control.cpp # SoftAP initialization & network configuration
    ├── motor_control.h  # Motor driver interface
    ├── motor_control.cpp# Differential drive & LEDC PWM hardware control
    ├── http_server.h    # HTTP server interface
    └── http_server.cpp  # WebServer routing, JSON parsing, failsafe watchdog & CORS
```

---

## Build & Installation

### Option A: Using PlatformIO (Recommended)

1. Clone this repository:
   ```bash
   git clone https://github.com/Dhaksith-S/RC-CAR.git
   cd RC-CAR
   ```
2. Open the project in VS Code with the PlatformIO extension installed (or use the PlatformIO CLI).
3. Connect your ESP32 board via USB.
4. Build and flash the firmware:
   ```bash
   pio run -t upload
   ```
5. Monitor serial output:
   ```bash
   pio device monitor -b 115200
   ```

### Option B: Using Arduino IDE

1. Ensure the **ESP32 Board Package** (v3.0 or higher) is installed via the Boards Manager.
2. Install **ArduinoJson** (v7.x) via Library Manager.
3. Copy all files from `src/` into a single sketch folder (e.g., `esp32_rc_car/`) and rename `main.cpp` to `esp32_rc_car.ino`.
4. Select your ESP32 Dev Module and COM port, then click **Upload**.

> [!NOTE]
> This firmware utilizes the modern **ESP32 Arduino Core 3.x** LEDC API (`ledcAttach(pin, freq, res)` and `ledcWrite(pin, duty)`). Ensure your build environment uses Core 3.x or above.

---

## Configuration

Customize project parameters in [`src/config.h`](file:///c:/Users/Dhaksith.S/esp32-rc-car/src/config.h):

```cpp
// Wi-Fi Access Point credentials
#define WIFI_AP_SSID          "RC-Car"
#define WIFI_AP_PASSWORD      "rccar1234"

// PWM Frequency and Resolution
#define PWM_FREQ_HZ           5000
#define PWM_RESOLUTION_BITS   8

// Speed limits
#define SPEED_MIN             0
#define SPEED_MAX             255
#define SPEED_DEFAULT_MAX     200     // Maximum speed limit

// Failsafe auto-stop timeout (ms)
#define COMMAND_TIMEOUT_MS    800     // Stops motors if no command received within 800ms
```

---

## How to Control the Car

1. **Power On**: Turn on the ESP32 and motor power supply.
2. **Connect**: On your smartphone or laptop, open Wi-Fi settings and connect to `RC-Car` using password `rccar1234`.
3. **Send Commands**:
   - Use any HTTP REST client, web-based joystick app, or custom controller.
   - Send periodic `POST /move` commands (at least once every 500 ms) to keep the car driving.
   - When control is released or disconnected, the failsafe mechanism automatically halts the car.

---

## License

This project is licensed under the [MIT License](LICENSE). Feel free to modify and adapt it for your own robotics and IoT projects!
