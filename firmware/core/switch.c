#include "switch.h"
#include "midi.h"
#include "pico/stdlib.h"

static const switch_t switches[SWITCH_CH_COUNT] = {
    { SWITCH_CH_1, 18 },
    { SWITCH_CH_2, 19 },
    { SWITCH_CH_3, 20 },
    { SWITCH_CH_4, 21 },
};

void switch_init(void) {
    for (int i = 0; i < SWITCH_CH_COUNT; ++i) {
        gpio_init(switches[i].pin);
        gpio_set_dir(switches[i].pin, GPIO_IN);
        gpio_pull_up(switches[i].pin);
    }
}

bool switch_get_state(switch_channel_t ch) {
    if (switch_validate_channel(ch)) {
        return !gpio_get(switches[ch].pin);
    }
    return false;
}

bool switch_validate_channel(switch_channel_t ch) {
    for (int i = 0; i < SWITCH_CH_COUNT; ++i) {
        if (switches[i].channel == ch) {
            return true;
        }
    }
    return false;
}