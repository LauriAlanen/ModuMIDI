// encoder.h
#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>
#include <stdint.h>

typedef enum { ENC_1 = 0, ENC_2, ENC_COUNT } encoder_id_t;

// Per-encoder data
typedef struct {
    uint8_t pin_a;
    uint8_t pin_b;
    volatile int32_t count;
    volatile uint8_t state; // previous A/B state (0..3)
} encoder_t;

// Public API
void encoder_init(void);
int32_t encoder_get_count(encoder_id_t id);

#endif // ENCODER_H
