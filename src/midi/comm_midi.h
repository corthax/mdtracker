#ifndef COMM_MIDI_H
#define COMM_MIDI_H

#include <genesis.h>

typedef enum {
    COMM_MIDI_NONE = 0,
    COMM_MIDI_EVERDRIVE_PRO,
    COMM_MIDI_EVERDRIVE_X7,
    COMM_MIDI_SERIAL,
} CommMidiMode;

void comm_midi_init(void);
bool comm_midi_read_ready(void);
u8   comm_midi_read(void);
CommMidiMode comm_midi_mode(void);

#endif
