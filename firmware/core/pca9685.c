#include "pca9685.h"
#include "hardware/i2c.h"

void pca9685_init(i2c_inst_t* i2c, uint8_t address) {
    uint8_t data[2];
    data[0] = 0x00; // MODE1 register
    data[1] = 0x00; // Normal mode
    i2c_write_blocking(i2c, address, data, 2, false);
}

void pca9685_set_pwm(i2c_inst_t* i2c, uint8_t channel, uint16_t on, uint16_t off) {
    uint8_t data[5];
    data[0] = 0x06 + 4 * channel; // LEDn_ON_L register
    data[1] = on & 0xFF;          // ON low byte
    data[2] = (on >> 8) & 0xFF;   // ON high byte
    data[3] = off & 0xFF;         // OFF low byte
    data[4] = (off >> 8) & 0xFF;  // OFF high byte
    i2c_write_blocking(i2c, PCA9685_I2C_ADDRESS, data, sizeof(data), false);
}

void pca9685_set_pwm_freq(i2c_inst_t* i2c, uint16_t freq) {
    uint8_t prescale = (uint8_t)(25000000 / (4096 * freq) - 1);
    uint8_t data[3];
    data[0] = 0xFE;     // PRE_SCALE register
    data[1] = prescale; // Prescale value
    data[2] = 0x00;     // Restart mode
    i2c_write_blocking(i2c, PCA9685_I2C_ADDRESS, data, sizeof(data), false);
}