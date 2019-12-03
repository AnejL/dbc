#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <errno.h>

// xorg
#include<X11/Xlib.h>

// network interface stats
#include <sys/ioctl.h>
#include <linux/wireless.h>

#include <pulse/volume.h>
#include <pulse/pulseaudio.h>



#define length(A) 	(sizeof(A) / sizeof(A[0]))
#define STATUSARG 	status+offset, sizeof(status) - offset
#define WLAN 		"wlp3s0"
#define ETH			"enp0s31f6"

static Display *dpy;

// general purpouse helper functions
//
void quit(char *message){
    printf("Quitting! Err: %s\n", message);
    exit(1);
}

int readIntFile(char *path) {
    int fd;

    if (!(fd = open(path, O_RDONLY)))
        quit("couldn't open file...");

    char buf[1];

    if (read(fd, buf, 1) < 0)
        quit("trouble reading int file");

    close(fd);

    return buf[0] - 48;
}

int readShortIntFile(char *path) {
    int fd;
    int size = 20;

    if (!(fd = open(path, O_RDONLY)))
        quit("couldn't open file...");

    char buf[size];

    ssize_t rd = read(fd, buf, sizeof(buf));

    if (rd < 0)
        quit("trouble reading int file");

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

void setstatus(char *s){
    // only works for one display
    XStoreName(dpy, DefaultRootWindow(dpy), s);
    XSync(dpy, False);
}

// system status retrieving functions
//
int getdatetime(char *s, size_t size) {
    time_t now = time(NULL);

    if (now == -1)
        quit("time() failiure...");

    struct tm *ptm = localtime(&now);

    if (ptm == NULL)
        quit("localtime() failiure");

    return strftime(s, size, " | %a %d %b %Y %H:%M ", ptm);
    //return strftime(s, size, " | %a %d %b %Y %H:%M:%S ", ptm);
}

int getpower(char* s, size_t size) {

    if (isDir("/sys/class/power_supply/BAT0")){
        int b0, b1, capacity, charging;

        b0 = readShortIntFile("/sys/class/power_supply/BAT0/capacity");
        b1 = readShortIntFile("/sys/class/power_supply/BAT1/capacity");

		capacity = (b0 + b1) / 2;

        charging = readIntFile("/sys/class/power_supply/AC/online");

        return snprintf(s, size, "[ bat %s %d%% ]", charging ? "c" : "d", capacity);
    }
    else
        return snprintf(s, size, "[ plugged in ] ");
}


int getnetwork(char* s, size_t size) {
    int eon, won;

    eon = readIntFile("/sys/class/net/enp0s31f6/carrier");
    won = readIntFile("/sys/class/net/wlp3s0/carrier");


    if (eon) {
        return snprintf(s, size, "\uE0B3 Ethernet ");
    }
    else if (won) {
        struct iwreq req;
        strncpy(req.ifr_ifrn.ifrn_name, WLAN, 6);

        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        char ssidstr[32];
        char *ssid;
        ssid = calloc(32, sizeof(char));
        req.u.essid.pointer = ssid;
        req.u.essid.length = 32;

        if (ioctl(sockfd, SIOCGIWESSID, &req) == -1){
            snprintf(ssidstr, 32, "[ Unavailable ESSID ] ");
        }
        else {
            snprintf(ssidstr, req.u.essid.length + 1, "%s ", req.u.essid.pointer);
        }
        close(sockfd);
        free(ssid);

        return snprintf(s, size, "[ w %s ] ", ssidstr);
    }
    else {
        return snprintf(s, size, "[ Not Connected ] ");
    }
}

int getsysinfo(char* s, size_t size) {

    struct sysinfo si;
    sysinfo(&si);

    unsigned int uptime = si.uptime;
    unsigned int days, hours, mins;

    days = uptime / 86400;
    hours = (uptime / 3600) - (days * 24);
    mins = (uptime / 60) - (days * 1440) - (hours * 60);

    return snprintf(s, size, "[ up %dd %dh %dm ] ", days, hours, mins);
}

int getvolume(char* s, size_t size) {
    /*FILE *fp;
    char path[10];

    fp = popen("pactl list sinks | awk '$1==\"Volume:\" {print $5}' | tr -d '\n'", "r");

    while (fgets(path, sizeof(path), fp) != NULL) {
        printf("%s", path);
    }

    pclose(fp);

    */

    pa_cvolume *pc;

    pa_cvolume_init(pc);
    //pc = malloc(1024);




    pa_volume_t *v;
    v = malloc(1024);

    *v = PA_VOLUME_NORM;

    //v = malloc(1024);


    return snprintf(s, size, "v: 10%% ");
}

void updatestatus() {
    // status string buffer
    // how many characters is still safe to write
    char status[300];

    int offset = 0;
    offset += getvolume(STATUSARG);
    //offset += getsysinfo(STATUSARG);
    offset += getnetwork(STATUSARG);
    offset += getpower(STATUSARG);
    offset += getdatetime(STATUSARG);
    setstatus(status);

}

// main
//
int main() {
    initdisplay();

    for (;; sleep(1) ) {
        updatestatus();
	}

    updatestatus();
    XCloseDisplay(dpy);
    return 0;
}
