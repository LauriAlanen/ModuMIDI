// encoder.c
#include "encoder.h"
#include <pico/stdlib.h>

// Lookup table: index = (old_state<<2) | new_state
static const int8_t quad_fsm[16] = { 0, -1, +1, 0, +1, 0, 0, -1, -1, 0, 0, +1,
    0, +1, -1, 0 };

static encoder_t encoders[ENC_COUNT] = { { .pin_a = 8, .pin_b = 9, .count = 0, .state = 0 },
    { .pin_a = 16, .pin_b = 17, .count = 0, .state = 0 } };

static void encoder_isr(uint gpio, uint32_t events) {
    // Find which encoder triggered
    for (int i = 0; i < ENC_COUNT; i++) {
        encoder_t* enc = &encoders[i];
        if (gpio == enc->pin_a || gpio == enc->pin_b) {
            // Read both pins
            uint8_t a         = gpio_get(enc->pin_a);
            uint8_t b         = gpio_get(enc->pin_b);
            uint8_t new_state = (a << 1) | b;
            uint8_t idx       = (enc->state << 2) | new_state;
            enc->count += quad_fsm[idx];
            enc->state = new_state;
        }
    }
}

void encoder_init(void) {
    // Initialize each encoder
    for (int i = 0; i < ENC_COUNT; i++) {
        encoder_t* enc = &encoders[i];

        // Initialize GPIO pins
        gpio_init(enc->pin_a);
        gpio_init(enc->pin_b);
        gpio_set_dir(enc->pin_a, GPIO_IN);
        gpio_set_dir(enc->pin_b, GPIO_IN);
        gpio_pull_up(enc->pin_a);
        gpio_pull_up(enc->pin_b);

        // Read initial state
        uint8_t a  = gpio_get(enc->pin_a);
        uint8_t b  = gpio_get(enc->pin_b);
        enc->state = (a << 1) | b;

        // Attach ISR on both edges of A and B
        gpio_set_irq_enabled_with_callback(
        enc->pin_a, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, encoder_isr);
        gpio_set_irq_enabled(enc->pin_b, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }
}

int32_t encoder_get_count(encoder_ch_t ch) {
    if (ch >= ENC_COUNT) {
        return 0; // Invalid channel
    }

    if (encoders[ch].count > 127) {
        encoders[ch].count = 127;
    } else if (encoders[ch].count < 0) {
        encoders[ch].count = 0;
    }

    return encoders[ch].count;
}
