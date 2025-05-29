// firmware/core/encoder.h
#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>
#include <stdint.h>

// --- Configuration ---
// Define the number of encoders your project will use.
// Ensure this matches the size of the 'encoders_config' array in encoder.c

// --- Types ---
// Enum to identify encoder channels.
typedef enum {
    ENC_1 = 0,
    ENC_2,
    ENC_COUNT
} encoder_ch_t;

// Structure to hold configuration for each encoder (pins).
// This is now separate from the runtime state.
typedef struct {
    encoder_ch_t channel_id; // Identifier for this encoder
    uint8_t pin_a;           // GPIO pin for encoder channel A
    uint8_t pin_b;           // GPIO pin for encoder channel B
} encoder_config_t;

// Internal structure to hold runtime state for each encoder.
// This is kept private to encoder.c
/*
typedef struct {
    volatile int32_t count;   // Current accumulated count (can be positive or negative)
    volatile uint8_t state;   // Previous A/B state for FSM (0..3)
    // Add other state variables if needed, e.g., for acceleration
} encoder_runtime_state_t;
*/

// --- Public API Functions ---

// Initializes all configured encoders.
// This function will use the 'encoders_config' array defined in encoder.c.
void encoder_init(void);

// Gets the raw accumulated count for the specified encoder channel.
// This count can be positive or negative and is not clamped by this function.
// Returns 0 if the channel is invalid.
int32_t encoder_get_raw_count(encoder_ch_t ch);

// Gets the change in count (delta) since the last call to this function
// for the specified encoder channel, and then resets the internal raw count to 0.
// Useful for relative MIDI control where you only care about the change.
// Returns 0 if the channel is invalid.
int32_t encoder_get_delta_and_reset(encoder_ch_t ch);

// Resets the raw accumulated count of a specific encoder channel to zero.
void encoder_reset_raw_count(encoder_ch_t ch);

#endif // ENCODER_H
