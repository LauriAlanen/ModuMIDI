// firmware/core/midi.h
#ifndef _MIDI_H_
#define _MIDI_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// USB MIDI Cable Number
#define MIDI_CABLE_NUM 0

// MIDI Status Byte Bases (Channel gets ORed in)
#define MIDI_STATUS_NOTE_OFF 0x80
#define MIDI_STATUS_NOTE_ON 0x90
#define MIDI_STATUS_CONTROL_CHANGE 0xB0
#define MIDI_STATUS_PROGRAM_CHANGE 0xC0
// Add other status bytes as needed (e.g., Pitch Bend 0xE0)

// Function to send a MIDI Control Change message
void midi_send_cc(uint8_t channel, uint8_t cc_number, uint8_t value);

// Function to send a MIDI Note On message
void midi_send_note_on(uint8_t channel, uint8_t note_number, uint8_t velocity);

// Function to send a MIDI Note Off message
void midi_send_note_off(uint8_t channel, uint8_t note_number, uint8_t velocity);

// Function to send a MIDI Program Change message
void midi_send_program_change(uint8_t channel, uint8_t program_number);

// Function to drain incoming MIDI messages
void midi_drain_input(void);

#endif /* _MIDI_H_ */
