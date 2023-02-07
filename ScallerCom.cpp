//define for scallercom
#include "config.h"
#include "functions.h"
#include "ScallerCom.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "../config.h"
#include <stdio.h>
#include "pico/stdlib.h"

uint8_t frame_buffer[frame_buffer_size];
uint8_t actual_frame_position = 0;
bool transmission_started = false;

//functions
bool calculate_crc(uint8_t *buffer);
scaller_frame generate_struct(uint8_t *buffer);
void send_frame(scaller_frame *Scaller_Frame);
void generate_checksum(scaller_frame *Scaller_Frame);

void ScallerCom::init(){
    
}

void ScallerCom::setAddress(uint8_t address){
    this->module_address = address;
}

void ScallerCom::setType(DeviceType d_type){
    this->device_type = d_type;
}

void ScallerCom::setMode(DeviceMode d_mode){
    this->device_mode = d_mode;
}

void ScallerCom::set485pin(uint8_t pin){
    this->pin_rs485 = pin;
    gpio_init(this->pin_rs485);
    gpio_set_dir(this->pin_rs485, GPIO_OUT);
    gpio_put(this->pin_rs485, 0);
}

void ScallerCom::scallercom_read(){
    if (uart_is_readable(UART_SCALLER_DEVICE)){
        uint8_t incoming_byte = uart_getc(UART_SCALLER_DEVICE);
        if (incoming_byte == frame_start_byte && actual_frame_position == 0 && !transmission_started){ //start transmission
            for (uint8_t i = 0; i < frame_buffer_size; i++) frame_buffer[i] = 0; //clean frame
            transmission_started = true;
        }
        else if (incoming_byte == frame_stop_byte && 
        ((actual_frame_position >= frame_buffer_size) || 
        ((actual_frame_position >= frame_position_data_size) && (actual_frame_position >= (frame_buffer[frame_position_data_size] + 5)))) && 
        transmission_started){ //stop transmission
            actual_frame_position = 0;
            transmission_started = false;
            if (calculate_crc(frame_buffer)){
                scaller_frame Scaller_Frame = generate_struct(frame_buffer);
                if (this->device_mode == MODE_SLAVE && (Scaller_Frame.address == this->module_address || Scaller_Frame.address == 0xffff)){
                    _callback(&Scaller_Frame);
                    generate_checksum(&Scaller_Frame);
                    send_frame(&Scaller_Frame); //send data
                }
                else if (this->device_mode == MODE_MASTER){
                    _callback(&Scaller_Frame);
                }
            }
        }
        else if (transmission_started){
            if (actual_frame_position >= frame_buffer_size ||
            ((actual_frame_position >= frame_position_data_size) && (actual_frame_position >= (frame_buffer[frame_position_data_size] + 5)))){
                actual_frame_position = 0;
                transmission_started = false;
            }
            else{
                frame_buffer[actual_frame_position] = incoming_byte;
                actual_frame_position++;
            }
        }
    }
}

void ScallerCom::send(scaller_frame *Scaller_Frame){
    generate_checksum(Scaller_Frame);
    send_frame(Scaller_Frame);
}

void ScallerCom::send_frame(scaller_frame *Scaller_Frame){
    if (this->pin_rs485 != 0xff){
        gpio_put(this->pin_rs485, 1);
        sleep_ms(4);
    }
    uart_putc(UART_SCALLER_DEVICE, frame_start_byte);
    uart_putc(UART_SCALLER_DEVICE, Scaller_Frame->address);
    uart_putc(UART_SCALLER_DEVICE, (Scaller_Frame->function >> 8) & 0xff);
    uart_putc(UART_SCALLER_DEVICE, Scaller_Frame->function);
    uart_putc(UART_SCALLER_DEVICE, Scaller_Frame->data_size);
    for (uint8_t i=0; i<Scaller_Frame->data_size; i++){
        uart_putc(UART_SCALLER_DEVICE, Scaller_Frame->data[i]);
    }
    uart_putc(UART_SCALLER_DEVICE, Scaller_Frame->checksum);
    uart_putc(UART_SCALLER_DEVICE, frame_stop_byte);
    uart_tx_wait_blocking(UART_SCALLER_DEVICE);
    // Serial.flush();
    if (this->pin_rs485 != 0xff){
         gpio_put(this->pin_rs485, 1);
    }
}

bool calculate_crc(uint8_t *buffer){
    uint16_t sum = 0;
    uint8_t data_size = buffer[frame_position_data_size];
    uint8_t checksum_pos = data_size + 4;
    for (uint8_t i=0; i < (frame_buffer_size - 1); i++){
        if (!(i > (data_size + 3))){
            sum = sum + buffer[i];
        }
    }
    if (sum % 255 == buffer[checksum_pos]){
        return true;
    }
    else {
        return false;
    }
}

scaller_frame generate_struct(uint8_t *buffer){
    uint8_t data_size = buffer[frame_position_data_size];
    scaller_frame Scaller_Frame;
    Scaller_Frame.address = buffer[frame_position_address];
    Scaller_Frame.function = buffer[frame_position_function + 1] | buffer[frame_position_function] << 8;
    Scaller_Frame.data_size = data_size;
    for (uint8_t i=0; i<data_size; i++){
        Scaller_Frame.data[i] = buffer[frame_position_data + i];
    }
    Scaller_Frame.checksum = buffer[data_size + 5]; 
    return Scaller_Frame;
}

void generate_checksum(scaller_frame *Scaller_Frame){
    uint16_t sum = 0;
    sum = Scaller_Frame->address;
    sum = sum + ((Scaller_Frame->function >> 8) & 0xff);
    sum = sum + (Scaller_Frame->function & 0xff);
    sum = sum + Scaller_Frame->data_size;
    for (uint8_t i=0; i<Scaller_Frame->data_size; i++){
        sum = sum + Scaller_Frame->data[i];
    }
    Scaller_Frame->checksum = sum % 0xff;
}