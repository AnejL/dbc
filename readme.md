# Yet another needlessly complicated status bar for dwm.

This program shows the current network status, alsa master volume, battery status and current date... and does it on multple threads.

I tried numerous times to implement PulseAudio module instead of ALSA, losing all hope eventually. Maybe I will try again some time in the future (if anyone knows how to do it efficiently please contact me).

This program was made and tested on the ThinkPad T480 and X230, both running Arch Linux. I have no guarantees that it will work on other laptops / operating systems even though it should. If you are interested in testing or have a problem, feel free to contact me at anej.lekse@gmail.com.

Sample output:

![alt text](https://github.com/anejl/dbc/blob/master/demo.png "Sample output")

## Installation:

**Dependencies:**

Arch Linux

> \# sudo pacman -S libx11 alsa-lib ttf-font-awesome

Create config.h file for storing local configurations

> sudo make

Reinstalling after changing config.h

> sudo make clean install

Uninstallation:

> sudo make clean uninstall

## Usage

Usage is extremely simple, **dbc** starts the statusbar script (you can put it in xinitrc, make a systemd service, etc.).

Command line options:

Execute **s**ingle iteration *(useful for timing, or cheesily modifying refresh interval)*

> dbc -s

**N**o error checking *(for people who like segfaults)*

> dbc -n

**O**utput status string **only once** to STDOUT *(useful for terminal-only SSH and maybe combining this program with shell scripts)*

> dbc -o

It's possible to combine them like

> dbc -sno

When you want to refresh the statusbar simply type **refbar** into the command line, your preferred program running tool (dmenu, rofi, etc.) or in your shell scripts.

## Configuration

After you run 

> sudo make 

installer generates config.h file. Edit the *quite self-explanatory* values in that file, **recompile** and run as usual.

## Combining dbc with shell scripts

It's perfectly possible to do something like this:

> echo "[ $( free -h | head -2 | tail -1 | awk '{print $3}') ] " | tr -d '\n' && dbc -o

Displaying the string would look something like this

> xsetroot -name " $HOSTNAME | $(dbc -o)"

## TODO:

- ensure no memory leaks
- optimise current features 
- add more modules and upgrade current ones
	- add default gateway ip to the output
	- disk space module
	- memory usage module
	- cpu usage module
	- compact view
- manpage entry
- configuration of control variables in config.h
