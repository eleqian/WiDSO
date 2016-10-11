/*
ringbuf.h

MIT License

Copyright (c) 2016 Yongqian Tang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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
