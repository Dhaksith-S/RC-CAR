#include "wifi_control.h"
#include <WiFi.h>
#include "config.h"

void wifiApInit() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_GATEWAY, WIFI_AP_SUBNET);
  bool ok = WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);

  Serial.println();
  Serial.println(ok ? "Wi-Fi AP started" : "Wi-Fi AP FAILED TO START");
  Serial.print("SSID: ");
  Serial.println(WIFI_AP_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}
