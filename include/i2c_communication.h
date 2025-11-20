#ifndef I2C_COMMUNICATION_H
#define I2C_COMMUNICATION_H

#include <cstdint>

#define I2C_BUFFER_SIZE 64

struct I2CMessage {
    uint8_t command;
    uint8_t data[I2C_BUFFER_SIZE];
    uint8_t checksum;
};

// I2C functions
void send_i2c_message();
uint8_t calc_checksum(const I2CMessage& msg);
bool send_message(const I2CMessage& msg);
bool receive_message(I2CMessage& msg);

#endif // I2C_COMMUNICATION_H