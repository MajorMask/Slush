// Device communication via I2C
#include "i2c_communication.h"
#include "error_handler.h"

void send_i2c_message() {
    log_entry("send_i2c_message");
    // ...I2C logic...
    log_exit("send_i2c_message");
}

#include <cstdint>
#define I2C_BUFFER_SIZE 64

struct I2CMessage {
    uint8_t command;
    uint8_t data[I2C_BUFFER_SIZE];
    uint8_t checksum;
};

// Calculate checksum
uint8_t calc_checksum(const I2CMessage& msg) {
    uint8_t sum = msg.command;
    for (int i = 0; i < I2C_BUFFER_SIZE; ++i) sum += msg.data[i];
    return sum;
}

// Send message
bool send_message(const I2CMessage& msg) {
    log_entry("send_message");
    // ...send via I2C bus...
    log_exit("send_message");
    return true;
}

// Receive message
bool receive_message(I2CMessage& msg) {
    log_entry("receive_message");
    // ...receive from I2C bus...
    log_exit("receive_message");
    return true;
}
