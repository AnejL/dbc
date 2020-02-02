# Yet another needlessly complicated status bar for dwm.

This program shows the current network status, alsa master volume, battery status and current date... and does it on multple threads.

I tried numerous times to implement PulseAudio module instead of ALSA, losing all the hope eventually. Maybe I will try again some time in the future (if anyone knows how to do it efficiently please contact me).

This program was made and tested on the ThinkPad T480 and X230, both running Arch Linux. I have no guarantees how it will work on other laptops / operating systems. If you are interested in testing or have a problem, feel free to contact me at anej.lekse@gmail.com.

## Installation:

>sudo make install

or, if you are reinstalling 

>sudo make clean install

Sample output:

>[ ((o)) homelan ] [ vol: 30% ] [ [+-] 100% ] | Thu 30 Jan 2020 01:32

## Usage

Usage is extremely simple, **dbc** starts the statusbar script (you can put it in xinitrc, make a systemd service, etc.).

When you want to refresh simply type **refbar** into the command line, your preferred program running tool (dmenu, rofi, etc.) or in your shell scripts.

## Configuration

Edit the values in the config.h header file, **recompile** and run as usual.

## How does it work?

> Initialise the display buffer


> MainLoop: 

> create thread for each module 

> write result of each function to an element of statusbuffer[] string array

> join the threads finally

> concatenate the separated elements to a single buffer

> write that buffer to xorg's XSetRoot buffer 

> repeat MainLoop

## TODO:

- fix makefile
- optimise further
