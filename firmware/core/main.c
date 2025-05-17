#include "bsp/board_api.h"
#include "tusb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encoder.h"
#include "midi.h"
#include "pca9685.h"
#include "pca9685_test.h"
#include "switch.h"

// TODO: Add a global map for MIDI controls

int main(void) {
    board_init();
    switch_init();
    encoder_init();
    pca9685_init(i2c_default, PCA9685_I2C_ADDRESS);

    uint16_t off = brightness_to_off(100);
    pca9685_set_pwm(i2c_default, PCA9685_I2C_ADDRESS, 14, off);
    pca9685_set_pwm(i2c_default, PCA9685_I2C_ADDRESS, 15, off);


    // init device stack on configured roothub port
    tusb_rhport_init_t dev_init = { .role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_AUTO };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    while (1) {
        tud_task(); // TinyUSB device task must be called periodically so blocking funcitions should limit their time
        if (switch_get_state(SWITCH_CH_4)) {
            midi_send_cc(SWITCH_CH_4, 127);
        } else {
            midi_send_cc(SWITCH_CH_4, 0);
        }

        int32_t count = encoder_get_count(ENC_1);
        if (count != 0) {
            midi_send_cc(ENC_1, count);
            encoder_get_count(ENC_1); // reset count
        }

        count = encoder_get_count(ENC_2);
        if (count != 0) {
            midi_send_cc(ENC_2, count);
            encoder_get_count(ENC_2); // reset count
        }
        midi_drain_input();
    }
}
