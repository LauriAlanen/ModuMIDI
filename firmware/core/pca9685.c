#include "pca9685.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

static uint32_t _oscillator_freq = PCA9685_OSCILLATOR_FREQ;

// Initialize PCA9685 for LED brightness control
void pca9685_init(i2c_inst_t* i2c, uint8_t address) {
    // Initialize I2C at 100kHz
    i2c_init(i2c, 100000);
#ifdef USE_I2C_DEFAULTS
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
#else
    gpio_set_function(PCA9685_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PCA9685_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PCA9685_SDA_PIN);
    gpio_pull_up(PCA9685_SCL_PIN);
#endif

    sleep_ms(10);
    // MODE1: restart, auto-increment, normal mode
    uint8_t data1[2] = { PCA9685_MODE1, MODE1_RESTART | MODE1_AI };
    i2c_write_blocking(i2c, address, data1, 2, false);
    sleep_ms(1);

    // MODE2: totem-pole outputs
    uint8_t data2[2] = { PCA9685_MODE2, MODE2_OUTDRV };
    i2c_write_blocking(i2c, address, data2, 2, false);
    sleep_ms(1);

    pca9685_set_pwm_freq(i2c_default, PCA9685_I2C_ADDRESS, PCA9685_DEFAULT_FREQ); // 1000 Hz
}

// Write single register
static void pca9685_write_register(i2c_inst_t* i2c, uint8_t address, uint8_t reg, uint8_t value) {
    uint8_t buf[2] = { reg, value };
    i2c_write_blocking(i2c, address, buf, 2, false);
}

// Set PWM frequency (in Hz)
void pca9685_set_pwm_freq(i2c_inst_t* i2c, uint8_t address, uint16_t freq) {
    // Constrain freq
    if (freq < PCA9685_MIN_FREQ)
        freq = PCA9685_MIN_FREQ;
    if (freq > PCA9685_MAX_FREQ)
        freq = PCA9685_MAX_FREQ;
    float prescaleval = (_oscillator_freq / (freq * PCA9685_PWM_RESOLUTION)) - 1.0f;
    uint8_t prescale = (uint8_t)(prescaleval + 0.5f);

    // Enter sleep to set prescale
    uint8_t oldmode;
    // Read MODE1
    i2c_write_blocking(i2c, address, (uint8_t[]){ PCA9685_MODE1 }, 1, false);
    i2c_read_blocking(i2c, address, &oldmode, 1, false);

    uint8_t sleepmode = (oldmode & ~MODE1_RESTART) | (1 << 4);
    pca9685_write_register(i2c, address, PCA9685_MODE1, sleepmode);
    // Set prescale
    pca9685_write_register(i2c, address, PCA9685_PRESCALE, prescale);
    // Restore mode and restart
    pca9685_write_register(i2c, address, PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
    sleep_ms(1);
}

// Set PWM duty on a channel: off_time in [0..4095]
void pca9685_set_pwm(i2c_inst_t* i2c, uint8_t address, uint8_t channel, uint16_t off) {
    uint8_t buf[5];
    buf[0] = PCA9685_LED0_ON_L + 4 * channel;
    buf[1] = 0x00; // We will only need to control the off time
    buf[2] = 0x00;
    buf[3] = off & 0xFF;
    buf[4] = (off >> 8) & 0x0F;
    i2c_write_blocking(i2c, address, buf, 5, false);
}

// Convert brightness percent (0-PCA9685_BRIGHTNESS_MAX) to off count
uint16_t brightness_to_off(uint8_t percent) {
    if (percent >= PCA9685_BRIGHTNESS_MAX)
        return PCA9685_MAX_PWM;
    return (uint16_t)((percent * PCA9685_MAX_PWM) / PCA9685_BRIGHTNESS_MAX);
}