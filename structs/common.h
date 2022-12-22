#include <Arduino.h>

struct ACK_DATA {
    uint8_t address;
    uint8_t type;
    uint32_t version;
};
