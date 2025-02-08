Demo procedure
==============

1. Connect Raspberry Pi and give some time to boot. After about a minuto, `RoverNetwork` should be available. Connect to it with password `roverpilot`.

2. Open 4 terminals.


**Terminal 1 - Rover live video camera publication**

~~~bash
roverssh
cd rover-application-ng
./publish-video.sh
~~~

**Terminal 2 - Rover TC/TM bridge**

~~~bash
roverssh
cd rover-application-ng
./run-bridge.sh
~~~

Connect companion computer (Raspberry Pi) to controller computer (STM32). Screen should show received packets:

~~~
./run-bridge.sh 
Listening at: 5558
Press any key to stopReceived payload: 0x 00 00 00 00 74 00 01 0F E4 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
Received payload: 0x 01 00 00 00 74 00 01 0F E4 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
Received payload: 0x 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
Received payload: 0x 00 00 00 00 75 00 01 12 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
Received payload: 0x 01 00 00 00 75 00 01 12 3C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
...
~~~

**Terminal 3 - Ground Station PS4 control**

~~~bash
roverdir # Convenience aliance to change to directory for cloned repository
cd Python/
conda activate # (if not activated)
./run-ps4-control.sh
~~~


**Terminal 2 - Ground Station Vision Module**

~~~bash
roverdir # Convenience aliance to change to directory for cloned repository
conda activate # (if not activated)
cd Python/rovervision2/
sudo ./run.sh # For some reason GStreamer requires superuser (FIX this later)
~~~

3. Operation

Rover starts DISARMED. Press PS4 button "O" to ARM/DISARM.

~~~
IS_ARMED: True
Sending to host 192.168.4.1, port: 5558
Sent: 0x 40 3C 05 03 00 00 00 01 E6 0A  (10 bytes)
~~~

When ARMED, it accepts joystick commands.

4. Teardown

Cancel processes in terminal 1 and 2 and run from any of them:

~~~bash
sudo poweroff
~~~

Disconnect Raspberry Pi and LiPO batteries.


## Known issues

For some reason, the firmware halts at some point after receiving commands. As there is no watchdog or supervision unit, this can make the Rover go in an unpredicted direction. This is being investigated.