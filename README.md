# Victron_BlueSmart_IP22
## Using the BlueSmart IP22 Charger without Bluetooth.
## (c) by <img src="https://github.com/wasn-eu/CubeCell_Getting_Started/raw/master/images/wasn_logo.png" width=70>

- [Finding the right connection](#finding-the-right-connection)   
 -- [Inside the BlueSmart IP22 charger](#inside-the-bluesmart-ip22-charger)   
 -- [Capture of Data](#capture-of-data)   
- [Connecting to venus](#connecting-to-venus)   
 -- [Shematics of USB to TTL](#shematics-of-usb-to-ttl)    
 -- [Connected to venus](#connected-to-venus)   
 -- [Manual Change Charge Current](#manual-change-charge-current)    
 -- [Automatic Change Charge Current](#automatic-change-charge-current)    
 -- [Service Change Charge Current](#service-change-charge-current)     
- [Adapter](#adapter)   
 -- [Adapter PCB](#adapter-pcb)     

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

### Automatic Change Charge Current

This Script will read the Grid Power with modbus from your venus device.    
When the read power is negativ (feeding to grid) it calculates the needed charge current to compensate and send it to your charger.    
If you are not feeding to grid it will send a charging current of 0A.    
      
The script will read the Grid Power and sent the charge current every 10 seconds. If you would like another interval change the sleep value in seconds in the last line of the script.
      
just create the file change_charge_current.py in /home/root

```
#!/usr/bin/python
import serial
import time
from pymodbus.constants import Defaults
from pymodbus.constants import Endian
from pymodbus.client.sync import ModbusTcpClient as ModbusClient
from pymodbus.payload import BinaryPayloadDecoder

ser = serial.Serial("/dev/ttyUSB1", 19200)

while(True):
  current = 0.0
  Defaults.Timeout = 5
  Defaults.Retries = 5

  client = ModbusClient('192.168.88.173', port=502, timeout=3, unit_id=100)
  result = client.read_input_registers(822, 1)
  if not result.isError():
    decoder = BinaryPayloadDecoder.fromRegisters(result.registers, byteorder=Endian.Big)
    power=decoder.decode_16bit_int()
    if power < 0:
      current = power / 24 * (-1)
      print("Grid Power:    {0:.0f}W".format(power))
      print("Grid Current:  {0:.2f}A".format(current))

      numP1 = (int)(current * 10)
      numP2 = (0x70 - numP1) & 0xFF
      hexP1 = "%X" % (numP1)
      hexP2 = "%X" % (numP2)
      if len(hexP1) < 2:
        hexP1 += "0"
      msg =  ':8F0ED00' + hexP1[0] + hexP1[1] + '00' + hexP2[0] + hexP2[1] + '\n'
      print("VE.direct out: " + msg)
      ser.write(msg.encode())
    else:
      print("Charging off")
  else:
    print("Error:", result)

  time.sleep(30)
```
You need to to adjust the line:
```
result = client.read_input_registers(822, 1)
                                     ---
```
to fit your system.     
     
Here it a little list what to write for the value 822 in your setup:

```
L1  820
L2  821
L3  822
```

Now you need to run this script on startup of venus os.    

Make the script executable:
```
chmod a+x change_charge_current.py
```
    
Add the script to crontab.    
Executes this command:
```
crontab -e
````
Add this:
```
@reboot python /home/root/change_charge_current.py > /var/log/change_charge_current.log
```

just found a problem that the charger does not accept a current value of 0.     
the minimum value is 3A     
Now i need to find out how to turn the charging on and off.     

### Service Change Charge Current

In the directory ```bluesmart-charger``` are all files to install this automatic script as a service on venus os.    
First off, a big thanks to [mr-manuel](https://github.com/mr-manuel) that created a bunch of templates that made this possible.   

#### Config
Copy or rename the ```config.sample.ini``` to ```config.ini``` in the ```bluesmart-charger``` folder and change it as you need it.

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

I have just created a little adapter pcb with a level shifter for RX and TX.   
With this you can use a standard ve.direct cable:
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/main/Images/adapter_pcb.jpg?raw=true" width=500>
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/main/Images/adapter_3d.jpg?raw=true" width=500>
</p>
And there is a little case for it.
<p align="center">
<img src="https://github.com/wasn-eu/Victron_BlueSmart_IP22/blob/main/Images/adapter_case.jpg?raw=true" width=500>
</p>

All files needed are in the pcb folder.    

You can use a short (about 10cm) 6pin to 6pin flat ribbon cable with the 6pin sockets to connect the charger to the pcb.   
the cable can be routed outside at the right side of the battery terminals. 
