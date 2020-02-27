#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>

// config and definitions files
#include "config.h"
#include "db.h"

// general purpouse helper functions
//
int readIntFile(char *path)
{
    int fd;

    if (!(fd = open(path, O_RDONLY)))
        quit("couldn't open file...");

    char buf[1];

	// this is a hacky way to bypass trouble when reading invalid .../carrier file
	// on an interface that is hardware-killed (eg thinkpad x230)
    if (read(fd, buf, 1) < 0)
	{
		close(fd);
		return -1;
	}

    close(fd);
	
	// ascii value - 48 is the correct numeric value
    return buf[0] - 48;
}

int readShortIntFile(char *path)
{
    int fd;
    int size = 20;

    if (!(fd = open(path, O_RDONLY)))
        quit("couldn't open file...");

    char buf[size];

    ssize_t rd = read(fd, buf, sizeof(buf));

    if (rd < 0)
        quit("Trouble reading short int file");

    close(fd);

    return atoi(buf);
}

int isDir(char *path) {
	int fd;
    if ((fd = open(path, O_DIRECTORY)) == -1 )
        return 0;
    else {
        close(fd);
        return 1;
    }
}

// statusbar specific helper functions
// initialise connection to X server
void initdisplay(){
    if (!(dpy = XOpenDisplay(NULL)))
        quit("display can't be opened...");
}

// system status retrieving functions
//
void getdatetime(char* statbuf)
{
    // get current timestamp, map it to timer struct and format it
    time_t now = time(NULL);

    if (now == -1)
        quit("time() failiure...");

    struct tm *ptm = localtime(&now);

    if (ptm == NULL)
        quit("localtime() failiure");

	char* part;
	part = calloc(30, sizeof(char));

	if (MINIMALMODE)
		strftime(part, 30, "%d.%m.%Y %H:%M", ptm);
	else
    	strftime(part, 30, "%a %d %b %Y %H:%M", ptm);

	// a cheesy but efficient way to get my original style even though it's a bit dirty
	if (!STYLE)
		sprintf(statbuf, "| %s ", part);
	else
		sprintf(statbuf, delimeterformat, part);

	free(part);
}

void getpower(char* statbuf) {

    // if you choose to monitor batteries get capacity levels for max 2 batteries
    if (BATTERYCOUNT > 0)
    {
        int capacity;
		
        capacity = readShortIntFile(BAT0);

        if (BATTERYCOUNT > 1)
        {
			// division by 2 in binary
            capacity = (capacity + readShortIntFile(BAT1)) >> 1;
        }

		char *part;
		part = calloc(10, sizeof(char));
		if (MINIMALMODE)
		{
			sprintf(part, "%d", capacity);
		}
		else
		{
			switch (capacity / 25)
			{
				case 0:
					strcpy(part, "");
					break;
				case 1:
					strcpy(part, "");
					break;
				case 2:
					strcpy(part, "");
					break;
				default:
					strcpy(part, "");
					break;
			}

			sprintf(part, "%s %d%%", part, capacity); 	
		}

        sprintf(statbuf, delimeterformat, part);
		free(part);
    }
    else
	{
		if (MINIMALMODE)
			sprintf(statbuf, delimeterformat, "");
		else
			sprintf(statbuf, delimeterformat, " AC");
	}
}

void getnetwork(char* statbuf)
{
    int eon, won;

    // read and check the current active device
    eon = readIntFile(ETHERNETON);
    won = readIntFile(WLANON);

    if (eon)
	{
        if (MINIMALMODE)
			sprintf(statbuf, delimeterformat, "");
		else
			sprintf(statbuf, delimeterformat, " Connected");
	}
    else if (won > 0)
    {
        struct iwreq req;
        int sockfd;
        char ssidstr[32];
        char *ssid;

        // card identifier is user-defined wlan name
        strncpy(req.ifr_ifrn.ifrn_name, WLAN, 6);

        // open a socket and feed it previously defined name and ssid buffers
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        ssid = calloc(32, sizeof(char));
        req.u.essid.pointer = ssid;
        req.u.essid.length = 32;

        // unavailable essid is mostly for error checking and never really happens but is left here to prevent segfault
        if (ioctl(sockfd, SIOCGIWESSID, &req) == -1)
            sprintf(ssidstr, delimeterformat, "Unavailable ESSID");
        else
            snprintf(ssidstr, req.u.essid.length + 1, "%s ", req.u.essid.pointer);

        // clean up
        close(sockfd);
        free(ssid);

		char* part;
		part = calloc(36, sizeof(char));
		sprintf(part, " %s", ssidstr);

        sprintf(statbuf, delimeterformat, part);
		free(part);
    }
    else if (won == 0)
        sprintf(statbuf, delimeterformat, "  ");
	else
		sprintf(statbuf, delimeterformat, "Killed");
}

void getvolume(char* statbuf) {
    // select default master profile from alsa devices
    long min, max, volume = 0;
	int enabled;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = CARD;
    const char *selem_name = PROFILE;

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

	// if card is enabled write 1 to enabled
	snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_UNKNOWN, &enabled);
	
	// write actual volume range to int pointers
	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_get_playback_volume(elem, 0, &volume);
	snd_mixer_close(handle);
	
	// reuse variable for percentage instead of the current absolute value... whatever
	volume = ((double)volume / max) * 100;

	char *part;
	part = calloc(12, sizeof(char));

	if (!enabled)
		strcpy(part, "");
	else
	{
		switch (volume / 20)
		{
			case 0:
				strcpy(part, "");
				break;
			case 1:
				strcpy(part, "");
				break;
			default:
				strcpy(part, "");
				break;
		}
	}
	sprintf(part, "%s %d%%", part, volume);
	sprintf(statbuf, delimeterformat, part);
	free(part);
}

void getcapslock(char* statbuf) {
	if (!printtostdout)
	{
		// get xorgs keyboard state and mask to get caps state
		unsigned n;
		XkbGetIndicatorState(dpy, XkbUseCoreKbd, &n);
		sprintf(statbuf, delimeterformat, (n & 1) ? "AB"  : "ab" );
	}
	else
		// if capsmodule is enabled and x isn't running just set placeholder
		sprintf(statbuf, delimeterformat, "ab");
}

void setstatus(){
    // only works for one display right now
    status = calloc(100, sizeof(char));
    int i;

    strcpy(status, statusbuffer[0]);
    for (i = 1; i < MODCOUNT + CAPSMODULE; i++)
        strcat(status, statusbuffer[i]);

	if (! printtostdout)
	{
		XStoreName(dpy, DefaultRootWindow(dpy), status);
		XSync(dpy, False);
	}
	else
		fprintf(stdout, "%s\n", status);
}

void updatestatus()
{
    // create a thread for every module, and join all threads afterwards
    int i;
    for (i = 0; i < MODCOUNT + CAPSMODULE; i++)
        pthread_create(&threads[i], NULL, (void *) modules[i], statusbuffer[i]);

    for (i = 0; i < MODCOUNT + CAPSMODULE; i++)
        pthread_join(threads[i], NULL);

    setstatus();
}

void refreshstatus(int signo)
{
    updatestatus();
}

void siginthandler(int signo)
{
	// free all status strings, previously allocated
    int i;
    for ( i = 0; i < MODCOUNT + CAPSMODULE; i++)
        free(statusbuffer[i]);

	free(status);
	free(delimeterformat);

	// close connection to X server
	if (! printtostdout)
    	XCloseDisplay(dpy);

    printf("Freed the status buffers and closed connection to X, exiting.\n");
	exit(0);
}

void initstatusbuffer()
{
	// allocate 30 bytes to status string locations
    int i;
    for ( i = 0; i < MODCOUNT + CAPSMODULE; i++)
        statusbuffer[i] = calloc(30, sizeof(char));
}

void checkconfig()
{
	// check if battery count was right
	if (BATTERYCOUNT > 0)
	{
		if (! isDir(BAT0DIR))
			err("Battery 0 not detected! Quitting!");
		
		if (BATTERYCOUNT == 2)
		{
			if (! isDir(BAT1DIR))
				err("Battery 1 not detected! Quitting!");
		}
		else if (BATTERYCOUNT > 2)
			quit("More than two batteries not supported!");
	}

	// TODO check if wlan / ethernet are even enabled
	// check wlan and ethernet
	if (! isDir(WLANDIR))
	{
		printf("WiFi interface with name %s not found!\nUse \"ip a\" to get its name.\n", WLAN);
		quit("Configuration checking failed!");
	}	
	if (! isDir(ETHERNETDIR))
	{
		printf("Ethernet interface with name %s not found!\nUse \"ip a\" to get its name.\n", ETHERNET);
		quit("Configuration checking failed!");
	}
}


void parseargs(int argc, char* argv[])
{
	int i, j;
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			for (j = 1; j < strlen(argv[i]); j++)
			{
				switch (argv[i][j])
				{
					case 's': 
						singleiter = 1;
						break;
					case 'n':
						noerror = 1;
						break;
					case 'o':
						singleiter = 1;
						printtostdout = 1;
						break;
					default:
						printf("Argument option \"%c\" is not recognised!\n", argv[i][j]);
						quit("");
						break;
				}
			}
		}
		else
			quit("Argument syntax wrong! Example: dbc -sn");
	}
}

void initvisuals()
{
	// based on the configured value set the delimeter
	delimeterformat = calloc(10, sizeof(char));
	switch(STYLE)
	{
		case 0:
			strcpy(delimeterformat, "[ %s ] ");
			break;
		case 1:
			strcpy(delimeterformat, "| %s "); 
			break;
		default:
			strcpy(delimeterformat, "\ue0b3 %s ");
			break;
	}
}

// main
//
int main(int argc, char* argv[])
{
	singleiter = 0;
	noerror = 0;
	printtostdout = 0;
	
	// parse arguments and set control variables
	if (argc > 1)
		parseargs(argc, argv);

	// if printtostdout is not set write to xsetroot
	if (! printtostdout)
    	initdisplay();

	// if error checking is enabled check them
	if (! noerror)
		checkconfig();

    // refresh on SIGUSR clean up on SIGINT
    signal(SIGUSR1, refreshstatus);
    signal(SIGINT, siginthandler);

	// initialise status string memory locations for set number of modules
    initstatusbuffer();

	// initialise visual changes
	initvisuals();

    // fill the array with pointers to statusbar module functions
	int index;
	index = 0;

	if (CAPSMODULE)
    	modules[index++] = getcapslock;

    modules[index++] = getnetwork;
    modules[index++] = getvolume;
    modules[index++] = getpower;
    modules[index++] = getdatetime;

    if (singleiter)
        updatestatus();
    else
    {
        for (;; sleep(REFRESHINTERVAL))
            updatestatus();
    }

    return 0;
}
