/*
 * Copyright (c) 2015, Ioannis Glaropoulos
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
 \file
 *   Include file for the Contiki low-layer 802.3 stack (ETHERNETSTACK)
 *
 * Created: 5/1/2014 11:50:16 PM
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 
#ifndef ETHERNETSTACK_H_
#define ETHERNETSTACK_H_

#include "contiki.h"

#ifndef ETHERNET_STACK
#ifdef ETHERNET_CONF_STACK
#define ETHERNET_STACK ETHERNET_CONF_STACK
#else /* ETHERNET_CONF_STACK */
#define ETHERNET_STACK nullethernet_driver
#endif /* ETHERNET_CONF_STACK */
#endif /* ETHERNET_STACK */

#include "ethernet.h"

/** Ethernet driver structure */
extern const struct eth_driver ETHERNET_STACK;

/** Process event indicating connection establishment */
extern process_event_t ethernet_link_connect_event_message;

void ethernetstack_init(void);
#endif /* ETHERNETSTACK_H_ */