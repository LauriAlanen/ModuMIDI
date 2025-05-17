#ifndef PCA9685_TEST_H
#define PCA9685_TEST_H

#include "pca9685.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

void brightness_sweep(i2c_inst_t* i2c, uint8_t address, uint8_t channel);

#endif // PCA9685_TEST_H