#include "http_server.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include "motor_control.h"
#include "config.h"

static WebServer server(HTTP_PORT);
static unsigned long lastCommandTime = 0;
static bool failsafeLogged = true;   // true at boot so we don't log a spurious failsafe
static int currentSpeed = 0;
static String currentDirection = "stop";

static void sendJson(int code, JsonDocument &doc) {
  String out;
  serializeJson(doc, out);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(code, "application/json", out);
}

static void sendError(int code, const char *message) {
  JsonDocument doc;
  doc["status"] = "error";
  doc["message"] = message;
  sendJson(code, doc);
}

static void handleStatus() {
  JsonDocument doc;
  doc["status"] = "ok";
  doc["connected"] = true;
  doc["direction"] = currentDirection;
  doc["speed"] = currentSpeed;
  sendJson(200, doc);
}

// Maps a named direction + requested speed onto the two motors.
// This is intentionally the only place that knows about named directions —
// everything below it works purely in left/right speed terms, so swapping
// this for real joystick (leftMotor = fwd + turn, etc.) later is a small change.
static bool applyDirection(const String &direction, int speed) {
  int s = constrain(speed, SPEED_MIN, SPEED_DEFAULT_MAX);

  if (direction == "forward") {
    driveDifferential(s, s);
  } else if (direction == "backward") {
    driveDifferential(-s, -s);
  } else if (direction == "left") {
    driveDifferential(0, s);
  } else if (direction == "right") {
    driveDifferential(s, 0);
  } else if (direction == "stop") {
    stopMotors();
    s = 0;
  } else {
    return false;
  }

  currentDirection = direction;
  currentSpeed = s;
  return true;
}

static void handleMove() {
  if (!server.hasArg("plain")) {
    sendError(400, "Missing JSON body");
    return;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    sendError(400, "Invalid JSON");
    return;
  }

  if (!doc["direction"].is<const char*>()) {
    sendError(400, "Missing 'direction' field");
    return;
  }

  String direction = doc["direction"].as<String>();
  int speed = doc["speed"] | 0;   // default to 0 if missing/wrong type

  if (!applyDirection(direction, speed)) {
    sendError(400, "Invalid direction");
    return;
  }

  lastCommandTime = millis();
  failsafeLogged = false;

  Serial.print("COMMAND: ");
  Serial.println(direction);
  Serial.print("SPEED: ");
  Serial.println(currentSpeed);

  JsonDocument res;
  res["status"] = "ok";
  res["direction"] = currentDirection;
  res["speed"] = currentSpeed;
  sendJson(200, res);
}

static void handleStop() {
  stopMotors();
  currentDirection = "stop";
  currentSpeed = 0;
  lastCommandTime = millis();
  failsafeLogged = false;

  Serial.println("COMMAND: STOP");

  JsonDocument res;
  res["status"] = "ok";
  res["direction"] = "stop";
  res["speed"] = 0;
  sendJson(200, res);
}

// Lets a browser-based test client (Phase 3) call the API with fetch() without
// CORS getting in the way.
static void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}

static void handleNotFound() {
  sendError(404, "Not found");
}

void httpServerInit() {
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/move", HTTP_POST, handleMove);
  server.on("/move", HTTP_OPTIONS, handleOptions);
  server.on("/stop", HTTP_POST, handleStop);
  server.on("/stop", HTTP_OPTIONS, handleOptions);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  lastCommandTime = millis();
}

void httpServerHandle() {
  server.handleClient();
}

void failsafeCheck() {
  if (motorsAreMoving() && (millis() - lastCommandTime > COMMAND_TIMEOUT_MS)) {
    stopMotors();
    currentDirection = "stop";
    currentSpeed = 0;

    if (!failsafeLogged) {
      Serial.println("FAILSAFE: COMMAND TIMEOUT");
      Serial.println("MOTORS STOPPED");
      failsafeLogged = true;
    }
  }
}
