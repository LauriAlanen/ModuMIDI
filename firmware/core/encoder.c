// firmware/core/encoder.c
#include "encoder.h"
#include "pico/stdlib.h"
#include "hardware/sync.h" // For critical sections (save_and_disable_interrupts)

// --- Configuration ---
// IMPORTANT: Define the GPIO pins for each encoder here.
// This array MUST match the ENC_COUNT and encoder_ch_t enum in encoder.h.
// Ensure these pins correspond to your physical hardware wiring.
static const encoder_config_t encoders_config[ENC_COUNT] = {
    { .channel_id = ENC_1, .pin_a = 8,  .pin_b = 9  }, // Example: Encoder 1 on GP8 (A) and GP9 (B)
    { .channel_id = ENC_2, .pin_a = 16, .pin_b = 17 }, // Example: Encoder 2 on GP16 (A) and GP17 (B)
};

// --- Internal State ---
// Finite State Machine (FSM) lookup table for quadrature decoding.
// Index = (old_AB_state << 2) | new_AB_state
// Values: 0 (no change/invalid), +1 (clockwise), -1 (counter-clockwise)
static const int8_t quadrature_fsm_table[16] = {
     0, // 00 -> 00
    -1, // 00 -> 01 (CCW)
    +1, // 00 -> 10 (CW)
     0, // 00 -> 11 (Error/skip)
    +1, // 01 -> 00 (CW)
     0, // 01 -> 01
     0, // 01 -> 10 (Error/skip)
    -1, // 01 -> 11 (CCW)
    -1, // 10 -> 00 (CCW)
     0, // 10 -> 01 (Error/skip)
     0, // 10 -> 10
    +1, // 10 -> 11 (CW)
     0, // 11 -> 00 (Error/skip)
    +1, // 11 -> 01 (CW)
    -1, // 11 -> 10 (CCW)
     0  // 11 -> 11
};

// Runtime state for each encoder
typedef struct {
    volatile int32_t count; // Current accumulated count
    volatile uint8_t ab_state; // Previous A/B state (encoded as (A << 1) | B)
} encoder_runtime_state_t;

static encoder_runtime_state_t encoder_states[ENC_COUNT];

// --- Interrupt Service Routine (ISR) ---
// This single ISR handles interrupts for all configured encoder pins.
static void encoder_gpio_isr(uint gpio, uint32_t events) {
    // Iterate through configured encoders to find which one triggered the ISR
    for (int i = 0; i < ENC_COUNT; ++i) {
        if (gpio == encoders_config[i].pin_a || gpio == encoders_config[i].pin_b) {
            // This encoder (i) is associated with the triggered GPIO pin

            // Read current state of both pins for this encoder
            uint8_t current_a_val = gpio_get(encoders_config[i].pin_a);
            uint8_t current_b_val = gpio_get(encoders_config[i].pin_b);
            uint8_t new_ab_state = (current_a_val << 1) | current_b_val;

            // Determine change using the FSM
            // Index into FSM table: (previous_ab_state << 2) | new_ab_state
            int8_t change = quadrature_fsm_table[(encoder_states[i].ab_state << 2) | new_ab_state];
            encoder_states[i].count += change;

            // Update the stored state for the next transition
            encoder_states[i].ab_state = new_ab_state;
            return;
        }
    }
}

// --- Public API Functions ---

void encoder_init(void) {
    for (int i = 0; i < ENC_COUNT; ++i) {
        // Initialize runtime state
        encoder_states[i].count = 0;

        // Initialize GPIO pins for encoder A and B
        gpio_init(encoders_config[i].pin_a);
        gpio_set_dir(encoders_config[i].pin_a, GPIO_IN);
        gpio_pull_up(encoders_config[i].pin_a); // Enable internal pull-up resistors

        gpio_init(encoders_config[i].pin_b);
        gpio_set_dir(encoders_config[i].pin_b, GPIO_IN);
        gpio_pull_up(encoders_config[i].pin_b); // Enable internal pull-up resistors

        // Read initial A/B state
        uint8_t initial_a = gpio_get(encoders_config[i].pin_a);
        uint8_t initial_b = gpio_get(encoders_config[i].pin_b);
        encoder_states[i].ab_state = (initial_a << 1) | initial_b;

        // Enable GPIO interrupts for both pins on both rising and falling edges
        // The shared ISR 'encoder_gpio_isr' will handle events from any of these pins.
        // Note: The callback is set only once with the first pin, but it's a global callback for GPIO IRQs.
        // The IRQ enable flags are set per pin.
        gpio_set_irq_enabled_with_callback(encoders_config[i].pin_a,
                                           GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                           true,
                                           &encoder_gpio_isr);
        gpio_set_irq_enabled(encoders_config[i].pin_b,
                             GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                             true);
    }
}

int32_t encoder_get_raw_count(encoder_ch_t ch) {
    if (ch >= ENC_COUNT) {
        return 0; // Invalid channel
    }
    
    int32_t current_count = encoder_states[ch].count;
    return current_count;
}

int32_t encoder_get_delta_and_reset(encoder_ch_t ch) {
    if (ch >= ENC_COUNT) {
        return 0; // Invalid channel
    }

    // Atomically read the current count and then reset it.
    // This critical section ensures the read and reset happen without interruption.
    uint32_t irq_status = save_and_disable_interrupts();
    int32_t delta = encoder_states[ch].count;
    encoder_states[ch].count = 0;
    restore_interrupts(irq_status);

    return delta;
}

void encoder_reset_raw_count(encoder_ch_t ch) {
    if (ch >= ENC_COUNT) {
        return; // Invalid channel
    }
    uint32_t irq_status = save_and_disable_interrupts();
    encoder_states[ch].count = 0;
    restore_interrupts(irq_status);
}
