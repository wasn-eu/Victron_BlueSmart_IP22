# Victron_BlueSmart_IP22
## Using the BlueSmart IP22 Charger without Bluetooth.
## (c) by <img src="https://github.com/wasn-eu/CubeCell_Getting_Started/raw/master/images/wasn_logo.png" width=70>

- [Finding the right connection](#finding-the-right-connection)   
  - [Inside the BlueSmart IP22 charger](#inside-the-bluesmart-ip22-charger)   
  - [Capture of Data](#capture-of-data)   
- [Connecting to venus](#connecting-to-venus)   
  - [Shematics of USB to TTL](#shematics-of-usb-to-ttl)    
  - [Connected to venus](#connected-to-venus) 
- [Changing the Charging Current](#changing-the-charging-current)  
  - [Manual Change Charge Current](#manual-change-charge-current)    
  - [Service Change Charge Current](#service-change-charge-current)     
- [Adapter](#adapter)   
  - [Adapter PCB](#adapter-pcb)     

      
## Finding the right connection
### Inside the BlueSmart IP22 charger

If you open the case you find a 6 pin male header with 5 pin populated.  
You can find the pinout here or in the Images folder Victron_BlueSmart_pinout.jpg  

<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/raw/master/Images/Victron_BlueSmart_pinout.jpg" width=500>  
</p>

The used bluetooth controller is a Raytec MDBT40.
     
     
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
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/master/Images/Victron_BlueSmart_ftdi.jpg?raw=true" width=500>  
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
     
     
## Changing the Charging Current
### Manual Change Charge Current

just rewrote a little python script to change the charge current directly on the venus os raspberry.    
Thanks to [sean56688](https://community.victronenergy.com/users/46454/sean56688.html) from victron community for the original script.     


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
    
just found a problem that the charger does not accept a current value of 0.     
the minimum value is 3A     
Now i need to find out how to turn the charging on and off.  
    
     
### Service Change Charge Current

In the directory ```bluesmart-charger``` are all files to install this automatic script as a service on venus os.    
First off, a big thanks to [mr-manuel](https://github.com/mr-manuel) that created a bunch of templates that made this possible.   
     
#### Config
Copy or rename the ```config.sample.ini``` to ```config.ini``` in the ```bluesmart-charger``` folder and change it as you need it.    
These values can  be changed in the config file:     
- ip: IP of your venus device (DEFAULT: 127.0.0.1)
- phase: modbus service id for the phase your system is connected to (DEFAULT: 820)
- interface: the USB interface of the charger (DEFAULT: /dev/ttyUSB1)
- intervall: how often the charging current should be calculated and send to the charger (DEFAULT: 30 seconds)
- maxcurrent: max charging current to limit this value (DEFAULT: 12 A)
      
#### Install
Copy the ```bluesmart-charger``` folder to ```/data/etc``` on your Venus OS device

Run bash ```/data/etc/bluesmart-charger/install.sh``` as root

The daemon-tools should start this service automatically within seconds.
     
#### Uninstall
Run ```/data/etc/bluesmart-charger/uninstall.sh```
     
#### Restart
Run ```/data/etc/bluesmart-charger/restart.sh```
     
#### Debugging
The logs can be checked with ```tail -n 100 -F /data/log/bluesmart-charger/current | tai64nlocal```

The service status can be checked with svstat: ```svstat /service/bluesmart-charger```

This will output somethink like ```/service/bluesmart-charger: up (pid 5845) 185 seconds```

If the seconds are under 5 then the service crashes and gets restarted all the time. If you do not see anything in the logs you can increase the log level in ```/data/etc/bluesmart-charger/config.ini``` by changing ```logging = WARNING``` to ```logging = INFO``` or ```logging = DEBUG```
     
     
## Adapter
### Adapter PCB

I have just created a little adapter pcb with a level shifter and isolation for RX and TX.   
With this you can use a standard ve.direct cable:
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/master/Images/pcb_schematic.jpg?raw=true" width=500>
</p>
Here is how it looks like:
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/master/Images/adapter_pcb.jpg?raw=true" width=500>
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/master/Images/adapter_3d.jpg?raw=true" width=500>
</p>
And there is a little case for it.
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/master/Images/adapter_case.jpg?raw=true" width=500>
</p>

All files needed are in the pcb folder.    

You can use a short (about 10cm) 6pin to 6pin flat ribbon cable with the 6pin sockets to connect the charger to the pcb.   
the cable can be routed outside at the right side of the battery terminals. 
