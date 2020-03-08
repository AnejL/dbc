// number of modules in statusbars that doubles as number of threads
// if you don't add any of your own modules this shouldn't change
// caps lock is a special module and should not be counted in here
#define MODCOUNT		6

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

// helper macros
#define err(mess) { fprintf(stderr,"Error: %s.\n", mess); exit(1); }
#define quit(mess) { fprintf(stdout,"Quitting: %s.\n", mess); exit(1); }
#define DEBUG(mess) { if(debug) { fprintf(stderr, "Debug:\t%s\n", mess); } }  
#define DEBUGINT(num) { if(debug) fprintf(stderr, "Debug:\t%d\n", num); }  

// lock variables
#define ULINT			31
#define ULSTART			32

/* abandon all hope, ye who enter here
#define LOCK(level, lockatint) { \
if ( (lock % level == 0) || (lock == ULINT && lockatint == 0 ) || lock == ULSTART ) {} \
else \
	return; \
} \
*/

#define LOCK(level, lockatint) { \
if ( lock % level && (lock != ULINT || lockatint ) && lock != ULSTART ) \
	return; \
} \

#define LOCKSTART() { if ( lock != ULSTART ) return; }

// thread array (one thread for each module) and array of strings for each status module
pthread_t threads[MODCOUNT + HOSTNAMEMODULE];
char *statusbuffer[MODCOUNT + HOSTNAMEMODULE];

// array of function pointers argument is a single statusbuffer[] element
void (*modules[MODCOUNT + HOSTNAMEMODULE]) (char* statbuf);

// final status char* that gets written to xsetroot
char *status;

// X11 display
static Display *dpy;

// single iteration execution, error supressing and printing to stdout control variables
int singleiter, noerror, printtostdout, debug;

// address of module format string, used in every module and defined in initvisuals
char* delimeterformat;

// lock variable that controls which modules get set/updated
int lock;
