#ifndef SWITCH_H
#define SWITCH_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SWITCH_CH_1 = 0,
    SWITCH_CH_2,
    SWITCH_CH_3,
    SWITCH_CH_4,
    SWITCH_CH_COUNT
} switch_channel_t; // :contentReference[oaicite:0]{index=0}

typedef struct {
    switch_channel_t channel;
    uint8_t pin;
} switch_t; // :contentReference[oaicite:1]{index=1}

void switch_init(void);
bool switch_get_state(switch_channel_t ch);

#endif // SWITCH_H
