#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include "base.h"

typedef struct ring_buffer {
    void *buffer;   //缓冲区
    u32 size;       //大小
    u32 in;         //入口位置
    u32 out;        //出口位置
} ringbuf_t;

u32 ringbuf_init(ringbuf_t *ring_buf, void *buffer, u32 size);
u32 ringbuf_use_len(const ringbuf_t *ring_buf);
u32 ringbuf_is_full(const ringbuf_t *ring_buf);
u32 ringbuf_get(ringbuf_t *ring_buf, void *buffer, u32 size);
u32 ringbuf_put(ringbuf_t *ring_buf, const void *buffer, u32 size);
u32 ringbuf_putc(ringbuf_t *ring_buf, const u8 c);

#endif //__RINGBUF_H__
