#!/usr/bin/env python

from gi.repository import GLib  # pyright: ignore[reportMissingImports]
import platform
import logging
import sys
import os
import time
import json
import configparser  # for config/ini file
import _thread
import serial
from pymodbus.constants import Defaults
from pymodbus.constants import Endian
from pymodbus.client.sync import ModbusTcpClient as ModbusClient
from pymodbus.payload import BinaryPayloadDecoder

# get values from config.ini file
try:
    config_file = (os.path.dirname(os.path.realpath(__file__))) + "/config.ini"
    if os.path.exists(config_file):
        config = configparser.ConfigParser()
        config.read(config_file)
    else:
        print(
            'ERROR:The "'
            + config_file
            + '" is not found. Did you copy or rename the "config.sample.ini" to "config.ini"? The driver restarts in 60 seconds.'
        )
        sleep(60)
        sys.exit()

except Exception:
    exception_type, exception_object, exception_traceback = sys.exc_info()
    file = exception_traceback.tb_frame.f_code.co_filename
    line = exception_traceback.tb_lineno
    print(
        f"Exception occurred: {repr(exception_object)} of type {exception_type} in {file} line #{line}"
    )
    print("ERROR:The driver restarts in 60 seconds.")
    sleep(60)
    sys.exit()

# Get logging level from config.ini
# ERROR = shows errors only
# WARNING = shows ERROR and warnings
# INFO = shows WARNING and running functions
# DEBUG = shows INFO and data/values
if "DEFAULT" in config and "logging" in config["DEFAULT"]:
    if config["DEFAULT"]["logging"] == "DEBUG":
        logging.basicConfig(level=logging.DEBUG)
    elif config["DEFAULT"]["logging"] == "INFO":
        logging.basicConfig(level=logging.INFO)
    elif config["DEFAULT"]["logging"] == "ERROR":
        logging.basicConfig(level=logging.ERROR)
    else:
        logging.basicConfig(level=logging.WARNING)
else:
    logging.basicConfig(level=logging.WARNING)


# get interval
if "DEFAULT" in config and "interval" in config["DEFAULT"]:
    interval = int(config["DEFAULT"]["interval"])
else:
    interval = 30
logging.debug(f"Interval: {interval}") 

# get interface
if "DEFAULT" in config and "interface" in config["DEFAULT"]:
    interface = config["DEFAULT"]["interface"]
else:
    interface = "/dev/ttyUSB1"
logging.debug(f"Interface: {interface}")

# get phase
if "DEFAULT" in config and "phase" in config["DEFAULT"]:
    phase = int(config["DEFAULT"]["phase"])
else:
    phase = 820
logging.debug(f"Phase: {phase}")

# get IP
if "DEFAULT" in config and "ip" in config["DEFAULT"]:
    ip = config["DEFAULT"]["ip"]
else:
    ip = "127.0.0.1"

def main():
  ser = serial.Serial(interface,19200)
  currentold = 0

  while(True):
    current = 0.0 
    chargerstate = 0.0
    Defaults.Timeout = 5
    Defaults.Retries = 5

#    client = ModbusClient(ip, port=502, timeout=3, unit_id=29)
#    result = client.read_input_registers(2318, 1)
#    if not result.isError():
#      decoder = BinaryPayloadDecoder.fromRegisters(result.registers, byteorder=Endian.Big)
#      chargerstate = decoder.decode_16bit_int()
#      logging.info("Charger State:       {0:.0f}".format(chargerstate))      
#    else:
#      logging.error("Error:", result)

    client = ModbusClient(ip, port=502, timeout=3, unit_id=100)
    result = client.read_input_registers(phase, 1)
    if not result.isError():
      decoder = BinaryPayloadDecoder.fromRegisters(result.registers, byteorder=Endian.Big)
      power = decoder.decode_16bit_int()
      if power < currentold:

        current = power / 24 * (-1)
        currentold = current
        current += currentold
        
        logging.info("Grid Power:        {0:.0f}W".format(power))
        logging.info("Grid Current last: {0:.2f}A".format(currentold))
        logging.info("Grid Current:      {0:.2f}A".format(current))
  
        numP1 = (int)(current * 10)
        numP2 = (0x70 - numP1) & 0xFF
        hexP1 = "%X" % (numP1)
        hexP2 = "%X" % (numP2)
        if len(hexP1) < 2:
          hexP1 += "0"
        msg =  ':8F0ED00' + hexP1[0] + hexP1[1] + '00' + hexP2[0] + hexP2[1] + '\n'
        logging.debug("VE.direct out: " + msg)
        ser.write(msg.encode())
      else:
        logging.info("Grid Power:    {0:.0f}W".format(power))
        logging.info("Charging off")
    else:
      logging.error("Error:", result)
  
    time.sleep(interval) 

if __name__ == "__main__":
    main() 
