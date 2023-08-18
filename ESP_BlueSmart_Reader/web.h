/***************************************************************************
  Copyright (c) 2023 WASN.eu

  Licensed under the MIT License. You may not use this file except in
  compliance with the License. You may obtain a copy of the License at

  https://opensource.org/licenses/MIT

***************************************************************************/

#ifndef _WEB_H
#define _WEB_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

#include "index_en.h" // HTML-Page (english)
#include "indexcss.h"
//#include "faviconico.h"

void startWebserver();
void stopWebserver();
void handleWebrequest();
void setMessage(const char *msg, uint8_t secs);

#endif