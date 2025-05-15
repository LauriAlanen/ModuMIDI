#ifndef SWITCH_H
#define SWITCH_H

#include <stdbool.h>
#include <stdint.h>

#define SWITCH_CH_COUNT 4
typedef enum {
    SWITCH_CH_1 = 0,
    SWITCH_CH_2 = 1,
    SWITCH_CH_3 = 2,
    SWITCH_CH_4 = 3,
} switch_channel_t;

typedef struct {
    switch_channel_t channel;
    uint8_t pin;
} switch_t;

void switch_init(void);
bool switch_get_state(switch_channel_t ch);
bool switch_validate_channel(switch_channel_t ch);

#endif // SWITCH_H
