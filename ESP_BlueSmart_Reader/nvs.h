/***************************************************************************
  Copyright (c) 2023 WASN.eu

  Licensed under the MIT License. You may not use this file except in
  compliance with the License. You may obtain a copy of the License at

  https://opensource.org/licenses/MIT

***************************************************************************/

#ifndef _NVS_H
#define _NVS_H

#include <Arduino.h>
#include <EEPROM_Rotate.h>
#include <ArduinoJson.h>

#define EEPROM_ADDR 10
#define BACKUP_CYCLE_MIN 60
#define BACKUP_CYCLE_MAX 180

typedef struct {
    uint16_t backupCycleMin;
    uint8_t readingsIntervalMs;
    bool enableMQTT;
    char mqttBroker[65];
    uint16_t mqttBrokerPort;
    char mqttBaseTopic[65];
    uint16_t mqttIntervalSecs;
    bool mqttEnableAuth;
    char mqttUsername[33];
    char mqttPassword[33];
    bool mqttJSON;
    bool enableHADiscovery;
    bool mqttSecure;
    bool enablePowerSavingMode;
    bool enableInflux;
    char systemID[17];
    uint8_t magic;
} settings_t; // (252*8) 2012 bytes (must be less than EEP's size, see nvs.c)

// use rotating pseudo EEPROM (actually ESP8266 flash)
extern settings_t settings;

void initNVS();
void saveNVS(bool rotate);
void resetNVS();
const char* nvs2json();
bool json2nvs(const char* buf, size_t size);

#endif