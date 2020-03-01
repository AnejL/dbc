#ifndef DBC_CONFIG_H
#define DBC_CONFIG_H

	// set to 1 if you want keymap module to show when you have capslock on [ us ] / [ US ] 
	#define CAPSMODULE		0

	// set to 1 if you want the first module to be your hostname if not, set to 0
	#define HOSTNAMEMODULE	1

	// number of seconds between the automatic refresh
	// set REFRESHINTERVAL to custom value
	#define REFRESHINTERVAL 1

    // number of batteries to monitor 
	// set BATTERYCOUNT to  0, 1 or 2
    #define BATTERYCOUNT   	0
   
    // network
	// set names of network devices to monitor (ip a)
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
	#define STYLE			1

	// turn shorter minimal mode on with 1 here
	#define MINIMALMODE		0
#endif //DBC_CONFIG_H
