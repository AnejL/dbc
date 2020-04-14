#ifndef DBC_CONFIG_H
#define DBC_CONFIG_H

	// set to 1 if you want keymap module to show when you have capslock on [ us ] / [ US ] 
    #define CAPSMODULE		0
	
	// set to 1 if you want the first module to be your hostname if not, set to 0
    #define HOSTNAMEMODULE	1

	// set to 1 if you want used memory module
    #define MEMMODULE	1

	// network
	// set names of network devices to monitor (ip a)
    #define WLAN            "wlp3s0"
    #define ETHERNET        "enp0s31f6"
    
#endif //DBC_CONFIG_H

// number of seconds between the automatic refresh
// set REFRESHINTERVAL to custom value
static const unsigned int REFRESHINTERVAL 		= 1;

// number of batteries to monitor 
// set BATTERYCOUNT to  0, 1 or 2
static const unsigned int BATTERYCOUNT	 		= 2;

// alsa
// set PROFILE to match your preferred soundcard profile e.g Master or IEC958
static const char* PROFILE 						= "Master";
static const char* CARD 						= "default";

// delimeter style
// 0 => [ mod ]
// 1 => | mod |
// 2 => < mod <
static const unsigned int STYLE 				= 1;

// turn shorter minimal mode on with 1 here
static const unsigned int MINIMALMODE 			= 0;
