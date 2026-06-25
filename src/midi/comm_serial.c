#include "comm_serial.h"
#include "ring_buf.h"
#include "serial.h"

// Serial port selection: IoPort_Ctrl2 (gamepad port 2) or IoPort_Ext (EXT/modem port)
#define COMM_SERIAL_PORT IoPort_Ctrl2
#define SERIAL_RING_BUF_SIZE 512

static u8 ring_buf_arr[SERIAL_RING_BUF_SIZE];
static ring_buf_t ring_buf;

static void update_buffer(void)
{
    while (serial_readyToReceive()) {
        ring_buf_write(&ring_buf, serial_receive());
    }
}

static void recv_ready_callback(void)
{
    update_buffer();
}

static void flush_rrdy(void)
{
    while (serial_readyToReceive()) {
        serial_receive();
    }
}

void comm_serial_init(void)
{
    ring_buf_init(&ring_buf, ring_buf_arr, sizeof(ring_buf_arr));
    serial_init(COMM_SERIAL_PORT, SCTRL_4800_BPS | SCTRL_SIN | SCTRL_SOUT | SCTRL_RINT);
    serial_setReadyToReceiveCallback(&recv_ready_callback);
    flush_rrdy();
}

bool comm_serial_is_present(void)
{
    return true;
}

u8 comm_serial_read_ready(void)
{
    return ring_buf_can_read(&ring_buf);
}

u8 comm_serial_read(void)
{
    u8 data = 0;
    ring_buf_status_t status = ring_buf_read(&ring_buf, &data);
    if (status == RING_BUF_OK) {
        return data;
    }
    return 0;
}

u8 comm_serial_write_ready(void)
{
    return serial_readyToSend();
}

void comm_serial_write(const u8* data, u16 length)
{
    for (u16 i = 0; i < length; i++) {
        while (!comm_serial_write_ready());
        serial_send(data[i]);
    }
}
