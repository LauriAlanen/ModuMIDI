// encoder.h
#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>
#include <stdint.h>

#define ENC_COUNT 2
typedef enum { ENC_1 = 0, ENC_2 = 1 } encoder_ch_t;

// Per-encoder data
typedef struct {
    uint8_t pin_a;
    uint8_t pin_b;
    volatile int32_t count;
    volatile uint8_t state; // previous A/B state (0..3)
} encoder_t;

// Public API
void encoder_init(void);
int32_t encoder_get_count(encoder_ch_t ch);

#endif // ENCODER_H
