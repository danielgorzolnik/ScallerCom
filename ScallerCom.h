#ifndef ForoCom_h
#define ForoCom_h
#include <Arduino.h>
#include "frame.h"
#include "structs/common.h"
#include "structs/relay.h"
#include "structs/input.h"
#include "enums.h"
#include "functions.h"

class ScallerCom{
    using my_callback = void (*)(scaller_frame *Foro_Frame);
    private:
        DeviceMode device_mode = MODE_SLAVE;
        my_callback _callback;
        byte pin_rs485 = 0xff;
        void send_frame(scaller_frame *);
    public:   
        byte module_address;
        DeviceType device_type = RELAY_8;
        void init();
        void send(scaller_frame *Scaller_Frame);
        void setType(DeviceType);
        void setMode(DeviceMode);
        void setAddress(byte);
        void scallercom_read();
        void set485pin(byte);
        void add_callback(my_callback get_callback)
        {
            _callback = get_callback;
        }
};

#endif