/***************************************************************************
  Copyright (c) 2023 WASN.eu

  Licensed under the MIT License. You may not use this file except in
  compliance with the License. You may obtain a copy of the License at

  https://opensource.org/licenses/MIT

***************************************************************************/

#include "config.h"
#include "victron.h"
#include "web.h"
#include "utils.h"
#include "influx.h"
#include "nvs.h"
#include "wlan.h"

EspSoftwareSerial::UART testSerial;


victron_t victron;

byte inbyte;
String incomingString;
float current;
float voltage;


// setup pin for IR sensor and allocate array for pulse readings
void initVictron() {
  testSerial.begin(19200, EspSoftwareSerial::SWSERIAL_8N1, A0, D5, false, 95, 11);
}


// scan ferraris disk for red marker and calibrate threshold
// returns true if system is calibrated and red marker was identified
bool readVictron() {
  if (testSerial.available()) inbyte = Serial.read();  //Serial input available
  switch (inbyte) {

    //====(Serial Menu)======
    case 'V':  //Display mode 0
      Serial.print("Voltage: ");
      if (testSerial.available()) {
        incomingString = testSerial.readStringUntil('\n');
      }
      voltage = incomingString.toFloat() / 1000;
      victron.voltage = voltage;
      Serial.print(incomingString.substring(-5));
      break;
    case 'I':  //Display mode 1
      Serial.print("Current: ");
      if (testSerial.available()) {
        incomingString = testSerial.readStringUntil('\n');
      }
      current = incomingString.toFloat() / 1000;
      victron.current = current;
      Serial.print(incomingString.substring(-4));
      break;
  }
  return true;
}