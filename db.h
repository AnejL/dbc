// number of modules in statusbars that doubles as number of threads
// if you don't add any of your own modules this shouldn't change
// caps lock is a special module and should not be counted in here
#define MODCOUNT		5

// battery dirs
#define BAT0DIR         "/sys/class/power_supply/BAT0"
#define BAT1DIR         "/sys/class/power_supply/BAT1"
#define BAT0            "/sys/class/power_supply/BAT0/capacity"
#define BAT1            "/sys/class/power_supply/BAT1/capacity"

// wlan and ethernet dirs
#define WLANDIR         "/sys/class/net/"WLAN
#define ETHERNETDIR     "/sys/class/net/"ETHERNET
#define WLANON          "/sys/class/net/"WLAN"/carrier"
#define ETHERNETON      "/sys/class/net/"ETHERNET"/carrier"

#define err(mess) { fprintf(stderr,"Error: %s.\n", mess); exit(1); }
#define quit(mess) { fprintf(stdout,"Quitting: %s.\n", mess); exit(1); }

// thread array (one thread for each module) and array of strings for each status module
// MODCOUNT + CAPSMODULE is used because caps lock module is not useful for everybody so it can be switched to 0
pthread_t threads[MODCOUNT + CAPSMODULE];
char *statusbuffer[MODCOUNT + CAPSMODULE];

// array of function pointers argument is a single statusbuffer[] element
void (*modules[MODCOUNT + CAPSMODULE]) (char* statbuf);

// final status char* that gets written to xsetroot
char *status;

// X11 display
static Display *dpy;

// single iteration execution, error supressing and printing to stdout control variables
int singleiter, noerror, printtostdout;

// address of module format string, used in every module and defined in initvisuals
char* delimeterformat;
