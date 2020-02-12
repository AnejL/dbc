# Yet another needlessly complicated status bar for dwm.

This program shows the current network status, alsa master volume, battery status and current date... and does it on multple threads.

I tried numerous times to implement PulseAudio module instead of ALSA, losing all hope eventually. Maybe I will try again some time in the future (if anyone knows how to do it efficiently please contact me).

This program was made and tested on the ThinkPad T480 and X230, both running Arch Linux. I have no guarantees that it will work on other laptops / operating systems even though it should. If you are interested in testing or have a problem, feel free to contact me at anej.lekse@gmail.com.

Sample output:

![alt text](https://github.com/anejl/dbc/blob/master/demo.png "Sample output")

## Installation:

> sudo make

Reinstalling after changing config.h

> sudo make clean install

Uninstallation:

> sudo make clean uninstall

## Usage

Usage is extremely simple, **dbc** starts the statusbar script (you can put it in xinitrc, make a systemd service, etc.).

When you want to refresh the statusbar simply type **refbar** into the command line, your preferred program running tool (dmenu, rofi, etc.) or in your shell scripts.

## Configuration

After you run 

> sudo make 

installer generates config.h file. Edit the values in that file, **recompile** and run as usual.

## TODO:

- optimise current features - especially reading of module files
- add more modules and upgrade current ones
	- add default gateway ip to the output
	- levels of charge, volume,...
	- disk space module
	- memory usage module
	- cpu usage module
	- compact view
