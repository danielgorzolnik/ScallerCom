enum DeviceMode {
    MODE_SLAVE,
    MODE_MASTER
};

enum DeviceType {
    //for master
    SERIAL_GATEWAY,
    ETHERNET_GATEWAY,
    WIRELESS_GATEWAY,

    //for slave
    RELAY_8,
    RELAY_16,
    INPUT_8,
    INPUT_16,
    TEMP_8,
    TEMP_16,
    BLIND_4,
    BLIND_8
};