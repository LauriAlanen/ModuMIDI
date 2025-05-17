#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encoder.h"
#include "midi.h"
#include "pca9685.h"
#include "switch.h"
#include "tusb.h"

/*------------- MAIN -------------*/
int main(void) {
    board_init();
    switch_init();
    encoder_init();

    // init device stack on configured roothub port
    tusb_rhport_init_t dev_init = { .role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_AUTO };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    while (1) {
        tud_task(); // TinyUSB device task
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
        midi_drain_input();
        sleep_ms(1);
    }
}
