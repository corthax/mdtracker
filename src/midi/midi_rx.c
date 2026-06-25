#include "midi_rx.h"
#include "midi_sync.h"
#include "comm_midi.h"

#define STATUS_LOWER(status) (status & 0x0F)
#define STATUS_UPPER(status) (status >> 4)

#define EVENT_NOTE_ON     0x9
#define EVENT_NODE_OFF    0x8
#define EVENT_CC          0xB
#define EVENT_PITCH_BEND  0xE
#define EVENT_PROGRAM     0xC
#define EVENT_SYSTEM      0xF

#define SYSTEM_CLOCK          0x8
#define SYSTEM_START          0xA
#define SYSTEM_STOP           0xC
#define SYSTEM_CONTINUE       0xB
#define SYSTEM_SONG_POSITION  0x2
#define SYSTEM_SYSEX          0x0
#define SYSTEM_RESET          0xF
#define SYSEX_END             0xF7

typedef u8 (*Reader)(void);

static u8 comm_read_byte(void);
static void read_message(Reader reader);
static void read_note_on(u8 status, Reader reader);
static void read_note_off(u8 status, Reader reader);
static void read_control_change(u8 status, Reader reader);
static void read_pitch_bend(u8 status, Reader reader);
static void read_program(u8 status, Reader reader);
static u16 read_14bit_value(Reader reader);
static void read_sysex(Reader reader);
static void read_system_message(u8 status, Reader reader);

static u8 comm_read_byte(void)
{
    return comm_midi_read();
}

void midi_rx_process(void)
{
    while (comm_midi_read_ready()) {
        read_message(comm_read_byte);
    }
}

static void read_message(Reader reader)
{
    u8 status = reader();
    u8 event = STATUS_UPPER(status);
    switch (event) {
    case EVENT_NOTE_ON:
        read_note_on(status, reader);
        break;
    case EVENT_NODE_OFF:
        read_note_off(status, reader);
        break;
    case EVENT_CC:
        read_control_change(status, reader);
        break;
    case EVENT_PITCH_BEND:
        read_pitch_bend(status, reader);
        break;
    case EVENT_PROGRAM:
        read_program(status, reader);
        break;
    case EVENT_SYSTEM:
        read_system_message(status, reader);
        break;
    default:
        break;
    }
}

static void read_control_change(u8 status, Reader reader)
{
    (void)status;
    reader();
    reader();
}

static void read_note_on(u8 status, Reader reader)
{
    (void)status;
    u8 pitch = reader();
    u8 velocity = reader();
    (void)pitch;
    if (velocity > 0) {
        midi_sync_note_trigger();
    }
}

static void read_note_off(u8 status, Reader reader)
{
    (void)status;
    reader();
    reader();
}

static void read_pitch_bend(u8 status, Reader reader)
{
    (void)status;
    read_14bit_value(reader);
}

static void read_program(u8 status, Reader reader)
{
    (void)status;
    reader();
}

static u16 read_14bit_value(Reader reader)
{
    u16 lower = reader();
    u16 upper = reader();
    return (upper << 7) + lower;
}

static void read_system_message(u8 status, Reader reader)
{
    u8 type = STATUS_LOWER(status);
    switch (type) {
    case SYSTEM_SONG_POSITION:
        read_14bit_value(reader);
        break;
    case SYSTEM_CLOCK:
        midi_sync_clock();
        break;
    case SYSTEM_START:
        midi_sync_start();
        break;
    case SYSTEM_CONTINUE:
        midi_sync_continue();
        break;
    case SYSTEM_STOP:
        midi_sync_stop();
        break;
    case SYSTEM_SYSEX:
        read_sysex(reader);
        break;
    case SYSTEM_RESET:
        break;
    default:
        break;
    }
}

static void read_sysex(Reader reader)
{
    const u16 BUFFER_LENGTH = 256;
    u8 buffer[BUFFER_LENGTH];
    u8 data;
    u16 index = 0;
    while (index < BUFFER_LENGTH && (data = reader()) != SYSEX_END) {
        buffer[index++] = data;
    }
    (void)buffer;
}
