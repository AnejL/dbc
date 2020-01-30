#ifndef DBC_CONFIG_H
#define DBC_CONFIG_H

    // number of modules in statusbars that doubles as number of threads
    #define MODCOUNT        4

    // number of batteries to monitor 0, 1 or 2
    #define BATTERYCOUNT    2
    #define BATDIR          "/sys/class/power_supply/BAT0"
    #define BAT0            "/sys/class/power_supply/BAT0/capacity"
    #define BAT1            "/sys/class/power_supply/BAT1/capacity"

    // network
    #define WLAN            "wlp3s0"
    #define ETHERNET        "enp0s31f6"
    #define WLANON          "/sys/class/net/"WLAN"/carrier"
    #define ETHERNETON      "/sys/class/net/"ETHERNET"/carrier"

    // alsa
    #define CARD            "default"
    #define PROFILE         "Master"

#endif //DBC_CONFIG_H
