#include <Arduino.h>

struct INPUT_STATUS {
    uint8_t status;
    uint8_t invert;
    uint8_t changed;
    uint8_t type;
    uint8_t relay[3];
};