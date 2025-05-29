// firmware/core/main.c

#include "bsp/board_api.h"
#include "tusb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "encoder.h"
#include "midi.h"         
#include "pca9685.h"
#include "pca9685_test.h"
#include "switch.h"
#include "midi_mapping.h"

// --- Global MIDI Control Map Instance ---
// (Ensure this is defined as per your requirements using the extended MidiControlMapping struct)
MidiControlMapping midi_control_map[] = {
    // Type, Input ID, Message Type, MIDI Ch, CC#, Note#, VelOn, VelOff, Prog#, SwOnVal, SwOffVal, MinOut, MaxOut
    {INPUT_TYPE_ENCODER, ENC_1, MIDI_MSG_TYPE_CONTROL_CHANGE, 1, 20, 0, 0, 0, 0, 0, 0, 0, 127},
    {INPUT_TYPE_ENCODER, ENC_2, MIDI_MSG_TYPE_CONTROL_CHANGE, 1, 21, 0, 0, 0, 0, 0, 0, 0, 127},
    {INPUT_TYPE_SWITCH, SWITCH_CH_4, MIDI_MSG_TYPE_CONTROL_CHANGE, 1, 22, 0, 0, 0, 0, 127, 0, 0, 127},
};
const int num_midi_mappings = sizeof(midi_control_map) / sizeof(MidiControlMapping);

// --- State for Encoder Parameters (Delta Method) ---
// Stores the current MIDI value for parameters controlled by encoders using the delta method.
// Size should match ENC_COUNT from encoder.h
// Initialized with a sentinel value to indicate they need to be set on first use.
int16_t encoder_parameter_values[ENC_COUNT]; // Stores the current value (0-127 typically)

// --- State for Switch Debouncing/Edge Detection ---
bool previous_switch_states[SWITCH_CH_COUNT]; // Assuming SWITCH_CH_COUNT is defined in switch.h

// Helper function to scale an input value to an output range (can be used for absolute ADC etc.)
// Not directly used for the core delta logic of encoders, but useful if you mix methods or for ADCs.
static uint8_t scale_value(int32_t input_value, int32_t input_min, int32_t input_max, uint8_t output_min, uint8_t output_max) {
    if (input_min == input_max) return output_min; // Avoid division by zero
    if (input_value <= input_min) return output_min;
    if (input_value >= input_max) return output_max;

    double ratio = (double)(input_value - input_min) / (input_max - input_min);
    int32_t scaled_value = output_min + (int32_t)floor(ratio * (output_max - output_min));

    if (scaled_value < output_min) scaled_value = output_min;
    if (scaled_value > output_max) scaled_value = output_max;
    return (uint8_t)scaled_value;
}


int main(void) {
    board_init();
    switch_init(); 
    encoder_init();
    pca9685_init(i2c_default, PCA9685_I2C_ADDRESS);

    tusb_rhport_init_t dev_init = { .role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_AUTO };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    // Initialize states
    for (int i = 0; i < ENC_COUNT; ++i) {
        encoder_parameter_values[i] = -1;
    }
    for(int i=0; i < SWITCH_CH_COUNT; ++i) { 
        previous_switch_states[i] = switch_get_state((switch_channel_t)i);
    }

    while (1) {
        tud_task(); // Essential TinyUSB task

        for (int i = 0; i < num_midi_mappings; i++) {
            MidiControlMapping* map = &midi_control_map[i];
            uint8_t final_midi_value; // The MIDI value to be sent (0-127)

            if (map->type == INPUT_TYPE_ENCODER) {
                int32_t delta = encoder_get_delta_and_reset((encoder_ch_t)map->input_id);

                if (delta != 0) { // Only process if there was actual movement
                    encoder_ch_t enc_channel = (encoder_ch_t)map->input_id;

                    if (encoder_parameter_values[enc_channel] == -1) {
                        encoder_parameter_values[enc_channel] = map->min_output_value;
                    }

                    // Apply delta to the current parameter value
                    int16_t new_param_value = encoder_parameter_values[enc_channel] + delta;
                    scale_value(new_param_value, map->min_output_value, map->max_output_value, 0, 127);
                    encoder_parameter_values[enc_channel] = new_param_value; // Store the updated value
                    final_midi_value = (uint8_t)new_param_value;

                    if (map->message_type == MIDI_MSG_TYPE_CONTROL_CHANGE) {
                        midi_send_cc(map->midi_channel, map->cc_number, final_midi_value);
                    }
                }
            } else if (map->type == INPUT_TYPE_SWITCH) {
                switch_channel_t switch_ch = (switch_channel_t)map->input_id; 
                bool current_switch_state = switch_get_state(switch_ch);

                if (current_switch_state != previous_switch_states[switch_ch]) { // State has changed
                    if (map->message_type == MIDI_MSG_TYPE_CONTROL_CHANGE) {
                        final_midi_value = current_switch_state ? map->switch_on_value : map->switch_off_value;
                        midi_send_cc(map->midi_channel, map->cc_number, final_midi_value);
                    }  else if (map->message_type == MIDI_MSG_TYPE_PROGRAM_CHANGE) {
                        if (current_switch_state) { 
                            midi_send_program_change(map->midi_channel, map->program_number);
                        }
                    }
                    previous_switch_states[switch_ch] = current_switch_state; // Update previous state
                }
            }
        }
        midi_drain_input();
    }
    return 0;
}
