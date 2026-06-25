#ifndef MIDI_SYNC_H
#define MIDI_SYNC_H

#include <genesis.h>

#define MIDI_CLOCKS_PER_BEAT  24
#define MIDI_CLOCKS_PER_PULSE  3

typedef enum {
    MIDI_SYNC_OFF = 0,
    MIDI_SYNC_CLOCK,
    MIDI_SYNC_NOTE,
} MidiSyncMode;

typedef enum {
    MIDI_TRANSPORT_NONE = 0,
    MIDI_TRANSPORT_START,
    MIDI_TRANSPORT_STOP,
    MIDI_TRANSPORT_CONTINUE,
} MidiTransportCmd;

extern bool bDoPulse;

void midi_sync_init(void);
void midi_sync_set_mode(MidiSyncMode mode);
MidiSyncMode midi_sync_get_mode(void);

void midi_sync_clock(void);
void midi_sync_start(void);
void midi_sync_continue(void);
void midi_sync_stop(void);
void midi_sync_note_trigger(void);

MidiTransportCmd midi_sync_get_transport(void);

#endif
