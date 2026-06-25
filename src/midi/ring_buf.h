#ifndef RING_BUF_H
#define RING_BUF_H

#include <genesis.h>

typedef enum { RING_BUF_OK = 0, RING_BUF_EMPTY, RING_BUF_FULL, RING_BUF_ERROR } ring_buf_status_t;

typedef struct {
    volatile u8* ringBuf;
    volatile u16 tail;
    volatile u16 head;
    u16 size;
} ring_buf_t;

void ring_buf_init(ring_buf_t* rb, u8* ringBuf, u16 size);
ring_buf_status_t ring_buf_write(ring_buf_t* rb, u8 data);
ring_buf_status_t ring_buf_read(ring_buf_t* rb, u8* data);
bool ring_buf_can_read(ring_buf_t* rb);
bool ring_buf_can_write(ring_buf_t* rb);
u16 ring_buf_available(ring_buf_t* rb);

#endif
