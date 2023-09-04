Raspberry PI Application Note - Manned teleoperated Rover
=========================================================

**Overview**

1. Basic software.
2. Remote operation.
2. Live video with GStreamer.
3. IMU / ROS integration


Basic software
---------------

### UART configuration.

1. **Backup Configuration Files:**

Before making any changes, it's a good practice to back up the configuration files you're going to modify. Run the following commands to create backups of the relevant files:

~~~bash
sudo cp /boot/cmdline.txt /boot/cmdline.txt.backup
sudo cp /etc/inittab /etc/inittab.backup
~~~

2. **Edit `/boot/cmdline.txt`:**

Use a text editor like nano or vim to edit the /boot/cmdline.txt file:

~~~bash
sudo nano /boot/cmdline.txt
~~~

Find the line that starts with `console=serial0,115200` (or similar) and remove it entirely. It should look something like this before the change:

~~~bash
console=serial0,115200
~~~

After removing the line, save the file and exit the text editor.

3. **Edit `/etc/inittab`:**

Edit the /etc/inittab file:

~~~bash
sudo nano /etc/inittab
~~~

Look for a line that contains something like `T0:23:respawn:/sbin/getty -L ttyAMA0 115200 vt100`. Comment out this line by adding a # at the beginning, like this:

~~~bash
#T0:23:respawn:/sbin/getty -L ttyAMA0 115200 vt100
~~~

Save the file and exit the text editor.

4. **Disable Serial Console Service:**

Stop and disable the serial console service using systemctl:

~~~bash
sudo systemctl stop serial-getty@ttyAMA0.service
sudo systemctl disable serial-getty@ttyAMA0.service
~~~

5. **Reboot:**

Reboot your Raspberry Pi for the changes to take effect:

~~~bash
sudo reboot
~~~

After performing these steps, the serial console on `/dev/ttyAMA0` should be disabled, and the UART will be available for communication with the STM32 microcontroller without interference from the operating system.

To re-enable the serial console for debugging or other purposes, you can reverse these steps by uncommenting the line in /etc/inittab and adding back the `console=serial0,115200` line in `/boot/cmdline.txt`.

### Start WiFI hotspot

To configure a Raspberry Pi as a WiFi hotspot using nmcli (NetworkManager Command-Line Interface), you can follow these steps. This guide assumes you have NetworkManager installed on your Raspberry Pi and that your Raspberry Pi has wireless capability (either built-in Wi-Fi or a compatible USB Wi-Fi adapter).

1. **Install NetworkManager if Not Installed:**

If NetworkManager is not already installed on your Raspberry Pi, you can install it using the following command:

~~~bash
sudo apt-get install network-manager
~~~

2. **Stop and Disable wpa_supplicant:**

NetworkManager will manage your Wi-Fi connections, so you should stop and disable wpa_supplicant, which is a separate service that may interfere with NetworkManager:

~~~bash
sudo systemctl stop wpa_supplicant
sudo systemctl disable wpa_supplicant
~~~

**3. Configure NetworkManager as a Hotspot:**

Use nmcli to create a new connection profile for your Wi-Fi hotspot. Replace `<SSID>` and `<PASSWORD>` with your desired network name (SSID) and password:

~~~bash
sudo nmcli connection add type wifi ifname wlan0 mode ap con-name MyHotspot ssid <SSID> 802-11-wireless-security.key-mgmt wpa-psk 802-11-wireless-security.psk <PASSWORD>
~~~

`wlan0` should be replaced with your actual Wi-Fi interface name if it's different.

4. **Enable and Start the Hotspot:**

Next, enable and start the hotspot connection:

~~~bash
sudo nmcli connection up MyHotspot
~~~

### Auto-start services on boot

To be completed.

Remote operation
----------------

Work in progress.


Live video
----------

See Gstramer scripts for video [publication](../Scripts/publish_video.sh) and [reception](../Scripts/receive_video.sh).

Integration with ROS (IMU telemetry + rovervision)
--------------------------------------------------

To be completed.
