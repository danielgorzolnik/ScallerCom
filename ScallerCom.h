#ifndef ForoCom_h
#define ForoCom_h
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
        uint8_t pin_rs485 = 0xff;
        void send_frame(scaller_frame *);
    public:   
        uint8_t module_address;
        DeviceType device_type = RELAY_8;
        void init();
        void send(scaller_frame *Scaller_Frame);
        void setType(DeviceType);
        void setMode(DeviceMode);
        void setAddress(uint8_t);
        void scallercom_read();
        void set485pin(uint8_t);
        void add_callback(my_callback get_callback)
        {
            _callback = get_callback;
        }
};

#endif