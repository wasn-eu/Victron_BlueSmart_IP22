/***************************************************************************
  Copyright (c) 2023 WASN.eu

  Licensed under the MIT License. You may not use this file except in
  compliance with the License. You may obtain a copy of the License at

  https://opensource.org/licenses/MIT

***************************************************************************/

#ifndef _VICTRON_H
#define _VICTRON_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#define METER_ID_LEN_MIN 2

typedef struct {
    float voltage;
    float current;
} victron_t;

extern victron_t victron;

void initVictron();
bool readVictron();


#endif
