/***************************************************************************
  Copyright (c) 2023 WASN.eu

  Licensed under the MIT License. You may not use this file except in
  compliance with the License. You may obtain a copy of the License at

  https://opensource.org/licenses/MIT

***************************************************************************/

#ifndef _WLAN_H
#define _WLAN_H

#include <ESP8266WiFi.h>
#include <WiFiManager.h>

extern uint16_t wifiReconnectCounter;
extern uint32_t wifiOnlineTenthSecs;
extern int8_t wifiStatus;

#define WIFI_AP_SSID "Victron_BlueSmart"
#define WIFI_MIN_RSSI 25
#define WIFI_CONFIG_TIMEOUT_SECS 300
#define WIFI_CONNECT_TIMEOUT 30

void startWifi();
void restartWifi();
void reconnectWifi();
void stopWifi(uint32_t currTime);

#endif
