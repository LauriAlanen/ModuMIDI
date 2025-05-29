// firmware/core/midi_mapping.h
#ifndef MIDI_MAPPING_H
#define MIDI_MAPPING_H

#include <stdint.h>
#include "encoder.h" // For encoder_ch_t 
#include "switch.h"  // For switch_channel_t

// Enum to define the type of physical input control
typedef enum {
    INPUT_TYPE_ENCODER,
    INPUT_TYPE_SWITCH,
    INPUT_TYPE_POTENTIOMETER // For analog inputs
} InputType;

// Enum to define the type of MIDI message to send
typedef enum {
    MIDI_MSG_TYPE_CONTROL_CHANGE,
    MIDI_MSG_TYPE_NOTE_ON,
    MIDI_MSG_TYPE_NOTE_OFF,
    MIDI_MSG_TYPE_PROGRAM_CHANGE
} MidiMessageType;

// Structure to define a single MIDI control mapping
typedef struct {
    InputType type;           // The type of the physical input (e.g., encoder, switch)
    uint8_t input_id;         // The specific ID of the input (e.g., ENC_1, SWITCH_CH_2, ADC_CHANNEL_0)
                              // This will be cast to the appropriate enum type

    // --- MIDI Message Configuration ---
    MidiMessageType message_type; // Type of MIDI message to generate
    uint8_t midi_channel;     // MIDI channel for this specific mapping (1-16).

    // --- Message-Specific Parameters ---
    // For Control Change (MIDI_MSG_TYPE_CONTROL_CHANGE)
    uint8_t cc_number;        // The MIDI CC number (0-127)

    // For Note On/Off (MIDI_MSG_TYPE_NOTE_ON, MIDI_MSG_TYPE_NOTE_OFF)
    uint8_t note_number;      // MIDI Note number (0-127)
    uint8_t velocity_on;      // Velocity for Note On (typically 1-127). For switches, this can be fixed.
                              // For analog/encoders, this might be mapped from input value or fixed.
    uint8_t velocity_off;     // Velocity for Note Off (typically 0-127, often 0 or 64).

    // For Program Change (MIDI_MSG_TYPE_PROGRAM_CHANGE)
    uint8_t program_number;   // Program Change number (0-127)

    // --- Input to Output Value Mapping ---
    // For Switches (used with any message type, e.g. to trigger a Note On with fixed velocity)
    uint8_t switch_on_value;  // Generic "value" when switch is ON.
                              // For CC: the CC value.
                              // For Note On: could be used as velocity if not using velocity_on.
                              // For Program Change: the program number.
    uint8_t switch_off_value; // Generic "value" when switch is OFF.
                              // For CC: the CC value.
                              // For Note Off: could be used as velocity if not using velocity_off.

    // For Encoders and Potentiometers (Analog Inputs)
    uint8_t min_output_value; // Minimum MIDI value to output (e.g., 0 for CC, velocity, program)
    uint8_t max_output_value; // Maximum MIDI value to output (e.g., 127 for CC, velocity, program)
                              // These help scale the raw input from an encoder or potentiometer
                              // to the desired MIDI data range.
} MidiControlMapping;

#endif // MIDI_MAPPING_H
