#include "midi_sync.h"

static MidiSyncMode syncMode = MIDI_SYNC_OFF;
static u8 clockCount = 0;
static MidiTransportCmd pendingTransport = MIDI_TRANSPORT_NONE;

void midi_sync_init(void)
{
    syncMode = MIDI_SYNC_OFF;
    clockCount = 0;
    pendingTransport = MIDI_TRANSPORT_NONE;
}

void midi_sync_set_mode(MidiSyncMode mode)
{
    syncMode = mode;
    clockCount = 0;
    pendingTransport = MIDI_TRANSPORT_NONE;
}

MidiSyncMode midi_sync_get_mode(void)
{
    return syncMode;
}

void midi_sync_clock(void)
{
    if (syncMode != MIDI_SYNC_CLOCK)
        return;

    clockCount++;

    if ((clockCount % MIDI_CLOCKS_PER_PULSE) == 0)
    {
        bDoPulse = TRUE;
    }

    if (clockCount >= MIDI_CLOCKS_PER_BEAT)
    {
        clockCount = 0;
    }
}

void midi_sync_start(void)
{
    if (syncMode == MIDI_SYNC_OFF)
        return;
    clockCount = 0;
    pendingTransport = MIDI_TRANSPORT_START;
}

void midi_sync_continue(void)
{
    if (syncMode == MIDI_SYNC_OFF)
        return;
    pendingTransport = MIDI_TRANSPORT_CONTINUE;
}

void midi_sync_stop(void)
{
    if (syncMode == MIDI_SYNC_OFF)
        return;
    clockCount = 0;
    pendingTransport = MIDI_TRANSPORT_STOP;
}

void midi_sync_note_trigger(void)
{
    if (syncMode != MIDI_SYNC_NOTE)
        return;

    bDoPulse = TRUE;
}

MidiTransportCmd midi_sync_get_transport(void)
{
    MidiTransportCmd cmd = pendingTransport;
    pendingTransport = MIDI_TRANSPORT_NONE;
    return cmd;
}
