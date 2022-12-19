#include <Arduino.h>

struct ACK_DATA {
    uint8_t address;
    uint8_t type;
    uint32_t version;
};

struct RELAY_STATUS {
    uint8_t status;
    uint8_t invert;
    uint8_t changed;
};