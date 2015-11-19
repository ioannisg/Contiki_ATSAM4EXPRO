/*
 * Copyright (c) 2008, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Ring buffer library implementation, 32-bit long pointers
 * \author
 *         Adam Dunkels <adam@sics.se>
 *         Ioannis Glaropoulos <ioannisg@kth.se>
 */

#include "contiki.h"
#include "lib/ringbuf32.h"
/*---------------------------------------------------------------------------*/
void
ringbuf32_init(struct ringbuf32 *r, uint8_t *dataptr, uint32_t size)
{
  r->data = dataptr;
  r->mask = size - 1;
  r->put_ptr = 0;
  r->get_ptr = 0;
}
/*---------------------------------------------------------------------------*/
int
ringbuf32_put(struct ringbuf32 *r, uint8_t c)
{
  /* Check if buffer is full. If it is full, return 0 to indicate that
     the element was not inserted into the buffer.
     XXX: there is a potential risk for a race condition here, because
     the ->get_ptr field may be written concurrently by the
     ringbuf_get() function. To avoid this, access to ->get_ptr must
     be atomic.
  */
  if(((r->put_ptr - r->get_ptr) & r->mask) == r->mask) {
    return 0;
  }
  CC_ACCESS_NOW(uint8_t, r->data[r->put_ptr]) = c;
  CC_ACCESS_NOW(uint8_t, r->put_ptr) = (r->put_ptr + 1) & r->mask;
  return 1;
}
/*---------------------------------------------------------------------------*/
int
ringbuf32_get(struct ringbuf32 *r)
{
  uint8_t c;

  /* Check if there are bytes in the buffer. If so, we return the
     first one and increase the pointer. If there are no bytes left, we
     return -1.
     XXX: there is a potential risk for a race condition here, because
     the ->put_ptr field may be written concurrently by the
     ringbuf_put() function. To avoid this, access to ->get_ptr must
     be atomic.
  */
  if(((r->put_ptr - r->get_ptr) & r->mask) > 0) {
    c = r->data[r->get_ptr];
    r->get_ptr = (r->get_ptr + 1) & r->mask;
    return c;
  } else {
    return -1;
  }
}
/*---------------------------------------------------------------------------*/
int
ringbuf32_size(struct ringbuf32 *r)
{
  return r->mask + 1;
}
/*---------------------------------------------------------------------------*/
int
ringbuf32_elements(struct ringbuf32 *r)
{
  return (r->put_ptr - r->get_ptr) & r->mask;
}
/*---------------------------------------------------------------------------*/