#ifndef _MIDI_H_
#define _MIDI_H_

#include <stdint.h>
#include <stdio.h>

#define MIDI_CH 0
#define MIDI_CABLE_NUM 0

#define MIDI_CC_CMD 0xB0

void midi_task(void);
void midi_send_cc(uint8_t cc, uint8_t value);
void midi_receive_cc(uint8_t cc, uint8_t value);

void led_blinking_task(void);

#endif /* _MIDI_H_ */