# dbc - yet another status bar for dwm written in C

This program shows these values by default:

- hostname (optional)
- used memory
- current network card status
- alsa volume
- power suppy status
- current X keymap
- and current date...

**...and does it on multple threads.**


Sample output:

![alt text](https://github.com/anejl/dbc/blob/master/demo.png "Sample output")

**dbc v2.0**

New version includes 3 delimeter styles to choose from and a minimal mode, useful for low-res displays. It also supports better configuration and it's much more optimised - it only updates the necessary modules when running idle; the hungrier ones get updated only at manual refreshes.

## Installation:

**Dependencies:**

Arch Linux

> \# sudo pacman -S libx11 alsa-lib ttf-font-awesome

**Create configuration files when running for the first time with:**

> sudo make

**Recompile everything after changing config.h for changes to take place with:**

> sudo make clean install

**Uninstallation:**

> sudo make clean uninstall

## Usage

Usage is extremely simple it consists of **two commands**, 

- **dbc** starts the statusbar *(you can put it in xinitrc, make a systemd service, etc.)*
- **refbar** refreshes the statusbar *(use that with your hotkey daemon)*

Command line options:

Execute **s**ingle iteration *(useful for timing)*

> dbc -s

**N**o error checking *(for testing purpouses more or less)*

> dbc -n

**O**utput status string **only once** to STDOUT *(useful for terminal-only SSH and maybe combining this program with shell scripts)*

> dbc -o

It's possible to combine options like

> dbc -sno

## Configuration

Installer generates config.h file. Edit the *quite self-explanatory* values in that file, **recompile** and run as usual.

## Combining dbc with shell scripts

It's perfectly possible to do something like this:

> echo "[ $( free -h | head -2 | tail -1 | awk '{print $3}') ] " | tr -d '\n' && dbc -o

Or

> xsetroot -name " $HOSTNAME | $(dbc -o)"

## TODO:

- optimise current features 
- add more modules and upgrade current ones
	- redo network module to read default route by itself 
- add configurable modules
- manpage entry
- test on other distros than Arch
- fix a bug with connecting usb wifi cards

### Contribution and hacking

This program was made and tested on the ThinkPad T480 and X230, both running Arch Linux. I have no guarantees that it will work on other laptops / distros even though it should. If you are interested in testing or have a problem, feel free to contact me.

I also tried numerous times to implement PulseAudio module instead of ALSA, losing all hope eventually. Maybe I will try again some time in the future (if anyone knows how to do it efficiently please contact me at anej.lekse@gmail.com).
