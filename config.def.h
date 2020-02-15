#ifndef DBC_CONFIG_H
#define DBC_CONFIG_H

    // number of modules in statusbars that doubles as number of threads
    #define MODCOUNT        4

    // number of batteries to monitor 0, 1 or 2
    #define BATTERYCOUNT    0
    #define BAT0DIR         "/sys/class/power_supply/BAT0"
    #define BAT1DIR         "/sys/class/power_supply/BAT1"
    #define BAT0            "/sys/class/power_supply/BAT0/capacity"
    #define BAT1            "/sys/class/power_supply/BAT1/capacity"

    // network
    #define WLAN            "wlp9s0"
    #define ETHERNET        "eno1"
    #define WLANDIR         "/sys/class/net/"WLAN
    #define ETHERNETDIR     "/sys/class/net/"ETHERNET
    #define WLANON          "/sys/class/net/"WLAN"/carrier"
    #define ETHERNETON      "/sys/class/net/"ETHERNET"/carrier"

    // alsa
    #define CARD            "default"
    #define PROFILE         "Master"

#endif //DBC_CONFIG_H