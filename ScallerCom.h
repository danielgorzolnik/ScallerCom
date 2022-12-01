#ifndef ForoCom_h
#define ForoCom_h
#include <Arduino.h>
#include "frame.h"
#include "structs.h"
#include "enums.h"

class ScallerCom{
    using my_callback = void (*)(scaller_frame *Foro_Frame);
    private:
        DeviceMode device_mode = MODE_SLAVE;
        my_callback _callback;
    public:   
        byte module_address;
        DeviceType device_type = RELAY_8;
        void init();
        void send(scaller_frame *Scaller_Frame);
        void setType(DeviceType);
        void setMode(DeviceMode);
        void setAddress(byte);
        void scallercom_read();
        void add_callback(my_callback get_callback)
        {
            _callback = get_callback;
        }
};

#endif