#include <Arduino.h>

struct RELAY_STATUS {
    uint8_t status;
    uint8_t invert;
    uint8_t changed;
};
