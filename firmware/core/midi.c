#include "midi.h"
#include "bsp/board_api.h"
#include "tusb.h"

void midi_send_cc(uint8_t cc, uint8_t value) {
    static uint8_t last_sent[128];
    static bool initialized = false;

    if (!initialized) {
        memset(last_sent, 0xFF, sizeof(last_sent));
        initialized = true;
    }

    if (last_sent[cc] == value) {
        return;
    }
    last_sent[cc] = value;

    uint8_t midi_packet[3] = { (uint8_t)(MIDI_CC_CMD | (MIDI_CH & 0x0F)), cc, value };
    tud_midi_stream_write(MIDI_CABLE_NUM, midi_packet, sizeof(midi_packet));
}

void midi_drain_input(void) {
    // The MIDI interface always creates input and output port/jack descriptors
    // regardless of these being used or not. Therefore incoming traffic should
    // be read (possibly just discarded) to avoid the sender blocking in IO
    uint8_t buf[4];
    // one loop, drains everything on all cables
    while (tud_midi_stream_read(buf, sizeof(buf)) > 0) {
        uint8_t cable = buf[0] >> 4;
        uint8_t cin   = buf[0] & 0x0F;
        // Just discard everything
    }
}