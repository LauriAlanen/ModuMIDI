#ifndef PCA9685_H
#define PCA9685_H

#include "hardware/i2c.h"
#include <stdint.h>

// defines and some logic from https://github.com/grzesiek2201/Adafruit-Servo-Driver-Library-Pi-Pico/blob/main/PCA9685_servo_driver.h
#define PCA9685_MODE1 0x00      /**< Mode Register 1 */
#define PCA9685_MODE2 0x01      /**< Mode Register 2 */
#define PCA9685_SUBADR1 0x02    /**< I2C-bus subaddress 1 */
#define PCA9685_SUBADR2 0x03    /**< I2C-bus subaddress 2 */
#define PCA9685_SUBADR3 0x04    /**< I2C-bus subaddress 3 */
#define PCA9685_ALLCALLADR 0x05 /**< LED All Call I2C-bus address */
#define PCA9685_LED0_ON_L 0x06  /**< LED0 on tick, low byte*/
#define PCA9685_LED0_ON_H 0x07  /**< LED0 on tick, high byte*/
#define PCA9685_LED0_OFF_L 0x08 /**< LED0 off tick, low byte */
#define PCA9685_LED0_OFF_H 0x09 /**< LED0 off tick, high byte */
// etc all 16:  LED15_OFF_H 0x45
#define PCA9685_ALLLED_ON_L 0xFA  /**< load all the LEDn_ON registers, low */
#define PCA9685_ALLLED_ON_H 0xFB  /**< load all the LEDn_ON registers, high */
#define PCA9685_ALLLED_OFF_L 0xFC /**< load all the LEDn_OFF registers, low */
#define PCA9685_ALLLED_OFF_H 0xFD /**< load all the LEDn_OFF registers,high */
#define PCA9685_PRESCALE 0xFE     /**< Prescaler for PWM output frequency */
#define PCA9685_TESTMODE 0xFF     /**< defines the test mode to be entered */

// MODE1 bits
#define MODE1_ALLCAL 0x01  /**< respond to LED All Call I2C-bus address */
#define MODE1_SUB3 0x02    /**< respond to I2C-bus subaddress 3 */
#define MODE1_SUB2 0x04    /**< respond to I2C-bus subaddress 2 */
#define MODE1_SUB1 0x08    /**< respond to I2C-bus subaddress 1 */
#define MODE1_SLEEP 0x10   /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20      /**< Auto-Increment enabled */
#define MODE1_EXTCLK 0x40  /**< Use EXTCLK pin clock */
#define MODE1_RESTART 0x80 /**< Restart enabled */
// MODE2 bits
#define MODE2_OUTNE_0 0x01 /**< Active LOW output enable input */
#define MODE2_OUTNE_1 \
    0x02 /**< Active LOW output enable input - high impedience */
#define MODE2_OUTDRV 0x04 /**< totem pole structure vs open-drain */
#define MODE2_OCH 0x08    /**< Outputs change on ACK vs STOP */
#define MODE2_INVRT 0x10  /**< Output logic state inverted */

#define PCA9685_I2C_ADDRESS 0x40
#define PCA9685_OSCILLATOR_FREQ 25000000.0f /**< 25MHz oscillator frequency */
#define PCA9685_PRESCALE_MIN 3
#define PCA9685_PRESCALE_MAX 255

#define PCA9685_MAX_PWM 4095
#define PCA9685_MIN_PWM 0

#define PCA9685_MAX_FREQ 1000
#define PCA9685_MIN_FREQ 1
#define PCA9685_DEFAULT_FREQ 60

#define USE_I2C_DEFAULTS
#define PCA9685_SDA_PIN 0
#define PCA9685_SCL_PIN 1

#define PCA9685_BRIGHTNESS_MAX 100
#define PCA9685_BRIGHTNESS_MIN 0
#define PCA9685_BRIGHTNESS_DEFAULT 50

#define PCA9685_PWM_RESOLUTION 4096.0f

void pca9685_init(i2c_inst_t* i2c, uint8_t address);
void pca9685_set_pwm(i2c_inst_t* i2c, uint8_t address, uint8_t channel, uint16_t off);
void pca9685_set_pwm_freq(i2c_inst_t* i2c, uint8_t address, uint16_t freq);
uint16_t brightness_to_off(uint8_t percent);
#endif // PCA9685_H