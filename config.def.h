#ifndef DBC_CONFIG_H
#define DBC_CONFIG_H

    // number of modules in statusbars that doubles as number of threads
	// if you don't add any of your own modules this shouldn't change
	// caps lock is a special module and should not be counted in here
    #define MODCOUNT		4

	// set to 1 if you want [ ab ] / [ AB ] to show when you have caps on
	// else set it to 0
	#define CAPSMODULE		0

	// number of seconds between the automatic refresh
	// set REFRESHINTERVAL to custom value
	#define REFRESHINTERVAL 20

    // number of batteries to monitor 
	// set BATTERYCOUNT to  0, 1 or 2
    #define BATTERYCOUNT   	0
   
    // network
	// set WLAN and ETHERNET variables
    #define WLAN            "wlp9s0"
    #define ETHERNET        "eno1"
    
    // alsa
	// set PROFILE to match your preferred soundcard profile e.g Master or IEC958
    #define CARD            "default"
    #define PROFILE         "Master"

	// delimeter style
	// 0 => [ mod ]
	// 1 => | mod |
	// 2 => < mod <
	#define STYLE			2

	// turn shorter minimal mode on with 1 here
	#define MINIMALMODE		0
#endif //DBC_CONFIG_H
