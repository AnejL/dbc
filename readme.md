##Yet another needlessly complicated status bar for dwm.

This program shows the user current network status, alsa volume, battery status and current date... and does it on multple threads.

###Installation:

>sudo make install

or, if you are reinstalling 

>sudo make clean install

Sample output:

>[ ((o)) homelan ] [ vol: 30% ] [ [+-] 100% ] | Thu 30 Jan 2020 01:32

TODO:

- fix makefile
- clean code
- check for possible memory leaks
- fix possible memory leaks
- optimise
