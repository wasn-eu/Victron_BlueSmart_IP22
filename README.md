# Victron_BlueSmart_IP22
## Using the BlueSmart IP22 Charger without Bluetooth.
## (c) by <img src="https://github.com/wasn-eu/CubeCell_Getting_Started/raw/master/images/wasn_logo.png" width=70>

- [Connecting to venus](#connecting-to-venus)   
  - [Connect USB to TTL Converter](#connect-usb-to-ttl-converter)    
  - [Connected to venus](#connected-to-venus) 
- [Service Change Charge Current](#service-change-charge-current)
  - [Install](#install)
  - [Config](#config)
  - [Uninstall](#uninstall)
  - [Restart](#restart)
  - [Debugging](#debugging)  
- [Adapter](#adapter)   
  - [Adapter PCB](#adapter-pcb)     
     
      
     
## Connecting to venus
### Connect USB to TTL Converter
     
![Victron FTDI Schematic](/Images/Victron_BlueSmart_ftdi.jpg)
      
On the picture the jumper for the TTL level is wrong. 
IMPORTANT: change the jumper to 3.3V    

You have to use TTL to USB adapter for 3.3V TTL level.   
If you use an isolated adapter you have to connect the 3.3V pin to the adapter, if not dont connect it.   
     
     
### Connected to venus

if you connect the ttl to usb cable direct to a venus os device (i have on here running on a raspberry pi 4) 
the charger will show and you can see all information about it:
  
![Screenshot01](../../raw/master/Images/ScreenShot_01.jpg)      
![Screenshot02](../../raw/master/Images/Screenshot_02.jpg)       
![Screenshot03](../../raw/master/Images/Screenshot_03.jpg) 
     
     
## Service Change Charge Current

In the directory ```bluesmart-charger``` are all files to install this automatic script as a service on venus os.    
First off, a big thanks to [mr-manuel](https://github.com/mr-manuel) that created a bunch of templates that made this possible.   
     
#### Install
Copy the ```bluesmart-charger``` folder to ```/data/etc``` on your Venus OS device
    
Run bash ```/data/etc/bluesmart-charger/install.sh``` as root

The daemon-tools should start this service automatically within seconds.
      
#### Config
Copy or rename the ```config.sample.ini``` to ```config.ini``` in the ```bluesmart-charger``` folder and change it as you need it.    
These values can  be changed in the config file:     

| **KEY** | **DESCRIPTION** | **DEFAULT** |
| :---: | :--- | :---: |
|  **ip** | IP of your venus device | 127.0.0.1 | 
|**phase**| modbus service id for the phase your system is connected to | 820 |
|**interface** | the USB interface of the charger | /dev/ttyUSB1 |
|**intervall** | how often the charging current should be calculated and send to the charger |30 seconds |
|**maxcurrent** | max charging current to limit this value | 12 A |

After changing the config file run the ```restart.sh``` script to activate the new config.     
     
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
![Adapter Schematic](/Images/pcb_schematic.jpg)
     
Here is how it looks like:       
![PCB](/Images/adapter_pcb.jpg)
![Adapter 3D](/Images/adapter_3d.jpg)
      
And there is a little case for it.      
![Adapter Case](/Images/adapter_case.jpg)
     

All files needed are in the pcb folder.    

You can use a short (about 10cm) 6pin to 6pin flat ribbon cable with the 6pin sockets to connect the charger to the pcb.   
the cable can be routed outside at the right side of the battery terminals.    

If you need a ready made one just drop me an email.
