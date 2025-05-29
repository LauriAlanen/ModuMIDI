// firmware/core/midi.c
#include "midi.h"
#include "bsp/board_api.h" // For board_millis or other board functions if needed
#include "tusb.h"          // For TinyUSB MIDI functions
#include <string.h>        // For memset

// --- Helper function to send a generic 3-byte MIDI message ---
static void send_midi_packet_3bytes(uint8_t status_byte_base, uint8_t channel, uint8_t data1, uint8_t data2) {
    // MIDI channels are 0-indexed in the status byte (0x00 - 0x0F)
    // So, subtract 1 from the 1-indexed channel argument.
    if (channel < 1 || channel > 16) return; // Invalid channel
    uint8_t status = (status_byte_base | ((channel - 1) & 0x0F));
    uint8_t midi_packet[3] = { status, data1, data2 };
    tud_midi_stream_write(MIDI_CABLE_NUM, midi_packet, sizeof(midi_packet));
}

// --- Helper function to send a generic 2-byte MIDI message ---
static void send_midi_packet_2bytes(uint8_t status_byte_base, uint8_t channel, uint8_t data1) {
    if (channel < 1 || channel > 16) return; // Invalid channel
    uint8_t status = (status_byte_base | ((channel - 1) & 0x0F));
    uint8_t midi_packet[2] = { status, data1 };
    tud_midi_stream_write(MIDI_CABLE_NUM, midi_packet, sizeof(midi_packet));
}


// --- Control Change ---
// Cache for CC messages to avoid sending redundant values
// Dimensions: [channel_index (0-15)][cc_number (0-127)]
static uint8_t last_sent_cc_values[16][128];
static bool cc_cache_initialized = false;

void midi_send_cc(uint8_t channel, uint8_t cc_number, uint8_t value) {
    if (!cc_cache_initialized) {
        for (int ch = 0; ch < 16; ch++) {
            memset(last_sent_cc_values[ch], 0xFF, sizeof(last_sent_cc_values[ch]));
        }
        cc_cache_initialized = true;
    }

    if (channel < 1 || channel > 16 || cc_number > 127 || value > 127) return;

    if (last_sent_cc_values[channel - 1][cc_number] == value) {
        return; // Value hasn't changed
    }
    last_sent_cc_values[channel - 1][cc_number] = value;

    send_midi_packet_3bytes(MIDI_STATUS_CONTROL_CHANGE, channel, cc_number, value);
}

// --- Note On ---
// Note: For simple Note On/Off triggered by switches, a state cache might be
// managed in main.c to know when to send Note Off.
// This function just sends the Note On message.
void midi_send_note_on(uint8_t channel, uint8_t note_number, uint8_t velocity) {
    if (note_number > 127 || velocity > 127) return; // Velocity 0 for Note On is sometimes treated as Note Off
    if (velocity == 0) { // As per MIDI spec, Note On with velocity 0 is a Note Off
        midi_send_note_off(channel, note_number, 0); // Or a default Note Off velocity like 64
        return;
    }
    send_midi_packet_3bytes(MIDI_STATUS_NOTE_ON, channel, note_number, velocity);
}

// --- Note Off ---
void midi_send_note_off(uint8_t channel, uint8_t note_number, uint8_t velocity) {
    if (note_number > 127 || velocity > 127) return;
    send_midi_packet_3bytes(MIDI_STATUS_NOTE_OFF, channel, note_number, velocity);
}

// --- Program Change ---
// Cache for Program Change messages
// Dimensions: [channel_index (0-15)]
static uint8_t last_sent_program_values[16];
static bool pc_cache_initialized = false;

void midi_send_program_change(uint8_t channel, uint8_t program_number) {
    if (!pc_cache_initialized) {
        memset(last_sent_program_values, 0xFF, sizeof(last_sent_program_values));
        pc_cache_initialized = true;
    }

    if (channel < 1 || channel > 16 || program_number > 127) return;

    if (last_sent_program_values[channel - 1] == program_number) {
        return; // Value hasn't changed
    }
    last_sent_program_values[channel - 1] = program_number;

    send_midi_packet_2bytes(MIDI_STATUS_PROGRAM_CHANGE, channel, program_number);
}


// --- Drain Input ---
void midi_drain_input(void) {
    uint8_t packet_buffer[4];
    while (tud_midi_available()) {
        tud_midi_packet_read(packet_buffer);
    }
}
