/**
 * \addtogroup linkaddr6
 * @{
 */

/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 *         Functions for manipulating Rime Ethernet addresses
 * \author
 *         Adam Dunkels <adam@sics.se>
 *			  Ioannis Glaropoulos <ioannisg@kth.se>
 */

#include "net/linkaddr6.h"
#include <string.h>

linkaddr6_t linkaddr6_node_addr;
linkaddr1_t linkaddr1_node_addr;

const linkaddr6_t linkaddr0_null = { { 0, 0, 0, 0, 0, 0 } };
const linkaddr1_t linkaddr1_null = { { 0, 0, 0, 0, 0, 0 } };


/*---------------------------------------------------------------------------*/
void
linkaddr6_copy(linkaddr6_t *dest, const linkaddr6_t *src)
{
	memcpy(dest, src, LINKADDR6_SIZE);
}
/*---------------------------------------------------------------------------*/
int
linkaddr6_cmp(const linkaddr6_t *addr1, const linkaddr6_t *addr2)
{
	return (memcmp(addr1, addr2, LINKADDR6_SIZE) == 0);
}
/*---------------------------------------------------------------------------*/
void
linkaddr6_set_node_addr(linkaddr6_t *t)
{
  linkaddr6_copy(&linkaddr6_node_addr, t);
}
/*---------------------------------------------------------------------------*/
void
linkaddr1_copy(linkaddr1_t *dest, const linkaddr1_t *src)
{
	memcpy(dest, src, LINKADDR1_SIZE);
}
/*---------------------------------------------------------------------------*/
int
linkaddr1_cmp(const linkaddr1_t *addr1, const linkaddr1_t *addr2)
{
	return (memcmp(addr1, addr2, LINKADDR1_SIZE) == 0);
}
/*---------------------------------------------------------------------------*/
void
linkaddr1_set_node_addr(linkaddr1_t *t)
{
  linkaddr1_copy(&linkaddr1_node_addr, t);
}
/*---------------------------------------------------------------------------*/
/** @} */