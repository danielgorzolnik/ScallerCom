#include <stdio.h>
#include <stdlib.h>
#include "config.h"

struct scaller_frame {
    uint8_t address;
    uint16_t function;
    uint8_t data_size;
    uint8_t data[frame_max_data_size];
    uint8_t checksum;
};