/***************************************************************************
  Copyright (c) 2023 WASN.eu

  Licensed under the MIT License. You may not use this file except in
  compliance with the License. You may obtain a copy of the License at

  https://opensource.org/licenses/MIT

***************************************************************************/

#ifndef _UTILS_H
#define _UTILS_H

#include <Arduino.h>

//#define DEBUG_HEAP

String systemID();
int32_t tsDiff(uint32_t tsMillis);
char* getRuntime(bool minutesOnly);
void blinkLED(uint8_t repeat, uint16_t pause);
void restartSystem();
void toggleLED();
void switchLED(bool state);

#endif