# Victron_BlueSmart_IP22
## Using the BlueSmart IP22 Charger without Bluetooth.
## (c) by <img src="https://github.com/wasn-eu/CubeCell_Getting_Started/raw/master/images/wasn_logo.png" width=70>

- [Finding the right connection](#finding-the-right-connection)   
 -- [Inside the BlueSmart IP22 charger](#inside-the-bluesmart-ip22-charger)   
 -- [Capture of Data](#capture-of-data)   
- [Connecting to venus](#connecting-to-venus)   
 -- [Shematics of USB to TTL](#shematics-of-usb-to-ttl)    
 -- [Connected to venus](#connected-to-venus)   
 -- [Manual_Change Charge Current](#manual-change-charge-current)    
- [Reading and Adapter](#reading-and-adapter)   
 -- [Adapter PCB](#adapter-pcb)   
 -- [ESP Reader](#esp-reader)   

## Finding the right connection
### Inside the BlueSmart IP22 charger

If you open the case you find a 6 pin male header with 5 pin populated.  
You can find the pinout here or in the Images folder Victron_BlueSmart_pinout.jpg  

<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/raw/master/Images/Victron_BlueSmart_pinout.jpg" width=500>  
</p>

The used bluetooth controller is a Raytec MDBT40.
You can find information about the module in the DataSheets folder.


### Capture of Data  
The signal level is 3.3V and the baudrate is 19200.  
After connecting a 3.3V TTL to USB converter to the RX/TX and GND pin i captured some data.  
THe capture is in the Captures folder.  
  
The Capture starts with a human readable header:  

```
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
```
   
and then there is much more data in hex format.   
It looks like that it is the ve.direct protocol.

You can read the battery voltage (V  28790) and charging current (I  5100).   
You have to divide the battery voltage and charging curent by 1000.   
In this example my battery have 28.79V and is currentlt=y charged with 5.1A.  

## Connecting to venus
### Shematics of USB to TTL

<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/main/Images/Victron_BlueSmart_ftdi.jpg?raw=true" width=500>  
</p>
On the picture the jumper for the TTL level is wrong. 
IMPORTANT: change the jumper to 3.3V    

You have to use TTL to USB adapter for 3.3V TTL level.   
If you use an isolated adapter you have to connect the 3.3V pin to the adapter, if not dont connect it.   


### Connected to venus

if you connect the ttl to usb cable direct to a venus os device (i have on ehere running on a raspberry pi 4) 
the charger will show and you can see all information about it:
  
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/raw/master/Images/ScreenShot_01.jpg" width=500>  
</p>
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/raw/master/Images/Screenshot_02.jpg" width=500>  
</p>
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/raw/master/Images/Screenshot_03.jpg" width=500>  
</p>
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/raw/master/Images/Screenshot_04.jpg" width=500>  
</p>

### Manual Change Charge Current

just wrote a little python script to change the charge current directly on the venus os raspberry:

name it charge_current.py

```
import sys
import serial
ser = serial.Serial("/dev/ttyUSB1", 19200)
chargeCurrent = int(sys.argv[1])

numP1 = (int)(chargeCurrent * 10)
numP2 = (0x70 - numP1) & 0xFF
hexP1 = "%X" % (numP1)
hexP2 = "%X" % (numP2)

# Write to serial port
msg =  ':8F0ED00' + hexP1[0] + hexP1[1] + '00' + hexP2[0] + hexP2[1] + '\n'
print(msg)
ser.write(msg.encode())
```

you have to change the serial device for your setup.

just run it with:

```
python charge_current.py X
```

where X is the needed charging current in A

Thanks to sean56688 from victron community



## Reading and Adapter
### Adapter PCB

I have just created a little adapter pcb with a level shifter for RX and TX.   
With this you can use a standard ve.direct cable:
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/main/Images/adapter_pcb.jpg?raw=true" width=500>  
</p>

All files needed are in the pcb folder.

### ESP Reader

in the ESP_BlueSmart_Reader folder is a Script for an ESP8266 thats reads voltage and current and publish them with mqtt.

