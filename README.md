# Victron_BlueSmart_IP22

Using the BlueSmart IP22 Charger without Bluetooth.

If you open the case you find a 6 pin male header with 5 pin populated.  
Until now i only know the GND Pin and the pin sending data to the bluetooth module.  
You can find the pinout here or in the Images folder Victron_BlueSmart_pinout.jpg  

<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/raw/master/Images/Victron_BlueSmart_pinout.jpg" width=500>  
</p>

  
The signal level is 3.3V and the baudrate is 19200.  
After connecting a 3.3V TTL to USB converter to the signal and GND pin i captured some data.  
THe capture is in the Captures folder.  
  
The Capture starts with a human readable header:  
  
PID     0xA334   
FWE     0342FF   
SER#    HQ2230XY6XR   
V       28790   
I       5100   
T       ---   
ERR     0   
CS      4   
HC#     1105659   
Checksum        :A0920000022   
   
and then there is much more data in hex format.   
I am still testing and evaluating what this data means.   
It looks like that it is the ve.direct protocol.
Hopefully i can find the rx pin on the charger side but had no luck yet.

   
But you can read the battery voltage (V  28790) and charging current (I  5100).   
You have to divide the battery voltage and charging curent by 1000.   
In this example my battery have 28.79V and is currentlt=y charged with 5.1A.  

in the ESP_BlueSmart_Reader folder is a Script for an ESP8266 thats reads voltage and current and publish them with mqtt.


