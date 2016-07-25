/*
 * Copyright (c) 2014, Swedish Institute of Computer Science.
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
 *         Ring memory library implementation
 * \author
 *         Ioannis Glaropoulos <ioannisg@sics.se>
 * Created: 2014-09-03 18:27:22
 */

#include "lib/ringmem.h"
#include "string.h"
#include "lib/assert.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define RINGMEM_STORE(_r, _ptr, _data, _len) \
  ((ringmem_block_t*)(&_r->data[_ptr]))->len = _len; \
  memcpy(&_r->data[_ptr+sizeof(uint16_t)], &_data[0], _len)
/*---------------------------------------------------------------------------*/
void
ringmem_init(struct ringmem *r, uint8_t *a, uint16_t len)
{
  r->size = ((len > RINGMEM_MAXSIZE) ? RINGMEM_MAXSIZE : len);
  r->data = a;
  r->a_len = r->size;
  r->put_ptr = 0;
  r->get_ptr = 0;
  r->wrap_ptr = 0;
}
/*---------------------------------------------------------------------------*/
int
ringmem_alloc(struct ringmem *r, uint8_t *data, uint16_t len)
{
  PRINTF("ringmem_alloc: request for: %u, available: %u current pointer: %u\n",
    len, r->a_len, r->put_ptr);

  uint32_t tot = len + sizeof(uint16_t);
  if (tot > r->a_len)
    return 0;

  /* Place only on consecutive space */
  tot += r->put_ptr;
  if (tot > r->size)
    goto wrap;

  /* Fits without wrap */
  RINGMEM_STORE(r, r->put_ptr, data, len);  
  r->put_ptr = tot;
  r->a_len -= len + sizeof(uint16_t);
  PRINTF("no wrap %u %u\n",r->put_ptr, r->a_len);
  return 1;
  	
wrap:
  tot = len + sizeof(uint16_t);
  if (tot > r->get_ptr)
    return 0;
	 
  /* Fits with wrap */
  r->wrap_ptr = r->put_ptr;
  r->put_ptr = 0;
  RINGMEM_STORE(r, r->put_ptr, data, len);  
  r->put_ptr = tot;
  r->a_len = r->get_ptr - r->put_ptr;
  PRINTF("wrap %u %u\n",r->put_ptr, r->a_len);
  return 1;
}
/*---------------------------------------------------------------------------*/
ringmem_block_t*
ringmem_get_next_block_ptr(struct ringmem *r)
{
  if (r->a_len == r->size)
    return NULL;
  
  if (r->get_ptr == r->wrap_ptr && r->wrap_ptr != 0) {
    r->get_ptr = 0;
	 r->wrap_ptr = 0;
  }
  PRINTF("get: %u\n",r->get_ptr);
  return (ringmem_block_t*)&r->data[r->get_ptr];    
}
/*---------------------------------------------------------------------------*/
void
ringmem_free_block(struct ringmem *r)
{
  if (r->a_len == r->size)
    assert(0);

  ringmem_block_t* b = (ringmem_block_t*)&r->data[r->get_ptr];

  r->get_ptr += b->len + sizeof(uint16_t);

  if (r->get_ptr == r->wrap_ptr && r->wrap_ptr != 0) {
    r->get_ptr = 0;
    r->wrap_ptr = 0;
  }
  
  if (r->get_ptr == r->put_ptr) {
    r->get_ptr = r->put_ptr = 0;
    r->a_len = r->size;

  } else if (r->get_ptr < r->put_ptr) {
    r->a_len = (r->size - r->put_ptr) + r->get_ptr;

  } else {
    r->a_len = r->get_ptr -r->put_ptr;
  }
  PRINTF("free: %u %u\n",r->get_ptr, r->a_len);
}
/*---------------------------------------------------------------------------*/