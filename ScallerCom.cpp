//define for scallercom
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "ScallerCom.h"
#include "config.h"
#include "functions.h"

// Soft serial
// SoftwareSerial softSerial (port_rx_pin, port_tx_pin);

byte frame_buffer[frame_buffer_size];
byte actual_frame_position = 0;
bool transmission_started = false;

//functions
bool calculate_crc(byte *buffer);
scaller_frame generate_struct(byte *buffer);
void send_frame(scaller_frame *Scaller_Frame);
void generate_checksum(scaller_frame *Scaller_Frame);

void ScallerCom::init(){
    Serial.begin(port_speed);
}

void ScallerCom::setAddress(byte address){
    this->module_address = address;
}

void ScallerCom::setType(DeviceType d_type){
    this->device_type = d_type;
}

void ScallerCom::setMode(DeviceMode d_mode){
    this->device_mode = d_mode;
}

void ScallerCom::set485pin(byte pin){
    this->pin_rs485 = pin;
    pinMode(this->pin_rs485, OUTPUT);
    digitalWrite(this->pin_rs485, LOW);
}

void ScallerCom::scallercom_read(){ //function is called every 1ms
    if (Serial.available() > 0){
        byte incoming_byte = Serial.read();
        if (incoming_byte == frame_start_byte && actual_frame_position == 0 && !transmission_started){ //start transmission
            for (byte i = 0; i < frame_buffer_size; i++) frame_buffer[i] = 0; //clean frame
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
        digitalWrite(this->pin_rs485, HIGH);
        _delay_ms(4);
    }
    Serial.write(frame_start_byte);
    Serial.write(Scaller_Frame->address);
    Serial.write((Scaller_Frame->function >> 8) & 0xff);
    Serial.write(Scaller_Frame->function);
    Serial.write(Scaller_Frame->data_size);
    for (byte i=0; i<Scaller_Frame->data_size; i++){
        Serial.write(Scaller_Frame->data[i]);
    }
    Serial.write(Scaller_Frame->checksum);
    Serial.write(frame_stop_byte);
    Serial.flush();
    if (this->pin_rs485 != 0xff) digitalWrite(this->pin_rs485, LOW);
}

bool calculate_crc(byte *buffer){
    uint16_t sum = 0;
    byte data_size = buffer[frame_position_data_size];
    byte checksum_pos = data_size + 4;
    for (byte i=0; i < (frame_buffer_size - 1); i++){
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

scaller_frame generate_struct(byte *buffer){
    byte data_size = buffer[frame_position_data_size];
    scaller_frame Scaller_Frame;
    Scaller_Frame.address = buffer[frame_position_address];
    Scaller_Frame.function = buffer[frame_position_function + 1] | buffer[frame_position_function] << 8;
    Scaller_Frame.data_size = data_size;
    for (byte i=0; i<data_size; i++){
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
    for (byte i=0; i<Scaller_Frame->data_size; i++){
        sum = sum + Scaller_Frame->data[i];
    }
    Scaller_Frame->checksum = sum % 0xff;
}