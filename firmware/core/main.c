/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board_api.h"
#include "midi.h"
#include "tusb.h"

#include "encoder.h"
#include "switch.h"


#define TOTAL_CONTROLS (SWITCH_CH_COUNT + ENC_COUNT)

// holds last‐sent CC values, init to 0xFF so first send always happens
static int16_t last_cc[TOTAL_CONTROLS];

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

        // Optional: throttle loop
        sleep_ms(10);
    }
}
