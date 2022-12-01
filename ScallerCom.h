#ifndef ForoCom_h
#define ForoCom_h
#include <Arduino.h>
#include "frame.h"
#include "enums.h"

class ScallerCom{
    using my_callback = void (*)(scaller_frame *Foro_Frame);
    private:
        byte module_address;
        DeviceMode device_mode = MODE_SLAVE;
        DeviceType device_type = RELAY_8;
        my_callback _callback;
    public:   
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