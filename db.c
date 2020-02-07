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
#include <sys/ioctl.h>
#include <linux/wireless.h>

#include "config.h"

#define err(mess) { fprintf(stderr,"Error: %s.\n", mess); exit(1); }

// thread array (one thread for each module) and array of strings for each status module
pthread_t threads[MODCOUNT];
char *statusbuffer[MODCOUNT];

// array of function pointers argument is a single statusbuffer[] element
void (*modules[MODCOUNT]) (char* statbuf);

// final status char* that gets written to xsetroot
char *status;

// X11 display
static Display *dpy;

// general purpouse helper functions
//
void quit(char *message)
{
    printf("Quitting! %s\n", message);
    exit(1);
}

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
    int fd = open(path, O_DIRECTORY);

    if (fd == -1 )
        return 0;
    else {
        close(fd);
        return 1;
    }
}

// statusbar specific helper functions
//
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

    strftime(statbuf, 80, "| %a %d %b %Y %H:%M ", ptm);
}

void getpower(char* statbuf) {

    // if you choose to monitor batteries get capacity levels for max 2 batteries
    if (BATTERYCOUNT > 0)
    {
        int b0;
		
        b0 = readShortIntFile(BAT0);

        if (BATTERYCOUNT > 1)
        {
			// division by 2 in binary
            b0 = (b0 + readShortIntFile(BAT1)) >> 1;
        }

        sprintf(statbuf, "[ [+-] %d%% ] ", b0);
    }
    else
        sprintf(statbuf, "[ AC ] ");
}


void getnetwork(char* stbf)
{
    int eon, won;

    // read and check the current active device
    eon = readIntFile(ETHERNETON);
    won = readIntFile(WLANON);

    if (eon)
        sprintf(stbf, "[ Ethernet ] ");
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
            snprintf(ssidstr, 32, "[ Unavailable ESSID ] ");
        else
            snprintf(ssidstr, req.u.essid.length + 1, "%s ", req.u.essid.pointer);

        // clean up
        close(sockfd);
        free(ssid);

        sprintf(stbf, "[ ((o)) %s ] ", ssidstr);
    }
    else if (won == 0)
        sprintf(stbf, "[ Not Connected ] ");
	else
		sprintf(stbf, "[ Killed ] ");
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

	// check if card is muted
	snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_UNKNOWN, &enabled);
	if (!enabled)
	{
    	sprintf(statbuf, "[ vol: x ] ");
	}
	else
	{
		// write actual volume range to int pointers
		snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
		snd_mixer_selem_get_playback_volume(elem, 0, &volume);
		snd_mixer_close(handle);
		
		// reuse variable for percentage instead of the current absolute value... whatever
		volume = ((double)volume / max) * 100;
    	sprintf(statbuf, "[ vol: %d%% ] ", volume);
	}

}

void setstatus(int limit){
    // only works for one display right now
    status = calloc(300, sizeof(char));
    int i;

    strcpy(status, statusbuffer[0]);
    for (i = 1; i < limit; i++)
    {
        strcat(status, statusbuffer[i]);
    }
    XStoreName(dpy, DefaultRootWindow(dpy), status);
    XSync(dpy, False);
}

void updatestatus()
{
    // create a thread for every module, and join all threads afterwards
    int i;
    for (i = 0; i < MODCOUNT; i++)
    {
        pthread_create(&threads[i], NULL, (void *) modules[i], statusbuffer[i]);
    }
    for (i = 0; i < MODCOUNT; i++)
        pthread_join(threads[i], NULL);

    setstatus(MODCOUNT);
}

void refreshstatus(int signo)
{
    updatestatus();
}

void siginthandler(int signo)
{
    int i;
    for ( i = 0; i < MODCOUNT; i++)
    {
        printf("%d\n", i);
        free(statusbuffer[i]);
    }
    free(status);

    printf("Freed the buffers, exiting.");

    XCloseDisplay(dpy);
}

void initstatusbuffer()
{
    int i;
    for ( i = 0; i < MODCOUNT; i++)
    {
        statusbuffer[i] = calloc(30, sizeof(char));
    }
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

// main
//
int main(int argc, char* argv[])
{
    initdisplay();

	checkconfig();

    // refresh on SIGUSR clean up on SIGINT
    signal(SIGUSR1, refreshstatus);
    signal(SIGINT, refreshstatus);

    initstatusbuffer();

    // fill the array with pointers to statusbar module functions
    modules[0] = getnetwork;
    modules[1] = getvolume;
    modules[2] = getpower;
    modules[3] = getdatetime;

    // if -s argument is passed at execution only set status once (for measuring time mostly)
    if (argc > 1 && strcmp(argv[1], "-s") == 0)
        updatestatus();
    else
    {
        for (;; sleep(20) ) {
            updatestatus();
        }
    }

    return 0;
}
