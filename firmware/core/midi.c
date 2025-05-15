#include "midi.h"
#include "bsp/board_api.h"
#include "tusb.h"

void midi_send_cc(uint8_t cc, uint8_t value) {
    uint8_t midi_packet[3] = { MIDI_CC_CMD | MIDI_CH, cc, value };
    tud_midi_stream_write(MIDI_CABLE_NUM, midi_packet, sizeof(midi_packet));
}

void midi_receive_cc(uint8_t cc, uint8_t value) {

    // The MIDI interface always creates input and output port/jack descriptors
    // regardless of these being used or not. Therefore incoming traffic should
    // be read (possibly just discarded) to avoid the sender blocking in IO
    while (tud_midi_available()) {
        uint8_t packet[4];
        tud_midi_packet_read(packet);
        if (packet[0] == MIDI_CC_CMD | MIDI_CH) {
            midi_receive_cc(packet[1], packet[2]);
        }
    }
}