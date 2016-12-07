# YunBitalino
Sourcecode for Arduino to read values from bitalino sensors (ecg and a respiration sensors) and publish them to a rest api.

# Setup

*   The sensors are connected to the analog pins directly (no Bitalino board in between) with the following setup <http://forum.bitalino.com/viewtopic.php?f=12&t=98#p159>
*   The python script is put on the Atheros 9331 part of the Yun
*   Install the requests library:
```bash
opkg update
opkg install distribute
opkg install python-openssl
easy_install pip
pip install requests
```

To improve the sensor signals I used filters created by this great site: <http://www.schwietering.com/jayduino/filtuino/>
