#include "comm_midi.h"
#include "comm_everdrive_pro.h"
#include "comm_everdrive.h"
#include "comm_serial.h"

static CommMidiMode activeMode = COMM_MIDI_NONE;

void comm_midi_init(void)
{
    comm_everdrive_pro_init();
    comm_everdrive_init();
    comm_serial_init();

    if (comm_everdrive_pro_is_present() && comm_everdrive_pro_read_ready())
        activeMode = COMM_MIDI_EVERDRIVE_PRO;
    else if (comm_everdrive_is_present() && comm_everdrive_read_ready())
        activeMode = COMM_MIDI_EVERDRIVE_X7;
    else
        activeMode = COMM_MIDI_SERIAL;
}

bool comm_midi_read_ready(void)
{
    switch (activeMode) {
    case COMM_MIDI_EVERDRIVE_PRO:
        return comm_everdrive_pro_read_ready();
    case COMM_MIDI_EVERDRIVE_X7:
        return comm_everdrive_read_ready();
    case COMM_MIDI_SERIAL:
        return comm_serial_read_ready();
    default:
        return false;
    }
}

u8 comm_midi_read(void)
{
    switch (activeMode) {
    case COMM_MIDI_EVERDRIVE_PRO:
        return comm_everdrive_pro_read();
    case COMM_MIDI_EVERDRIVE_X7:
        return comm_everdrive_read();
    case COMM_MIDI_SERIAL:
        return comm_serial_read();
    default:
        return 0;
    }
}

CommMidiMode comm_midi_mode(void)
{
    return activeMode;
}
