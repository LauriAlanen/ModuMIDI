#include "pca9685_test.h"

void brightness_sweep(i2c_inst_t* i2c, uint8_t address, uint8_t channel) {
    for (uint16_t i = 0; i < PCA9685_BRIGHTNESS_MAX; i++) {
        uint16_t off = brightness_to_off(i);
        pca9685_set_pwm(i2c, address, channel, off);
        sleep_ms(10);
    }
    for (uint16_t i = PCA9685_BRIGHTNESS_MAX; i > 0; i--) {
        uint16_t off = brightness_to_off(i);
        pca9685_set_pwm(i2c, address, channel, off);
        sleep_ms(10);
    }
}