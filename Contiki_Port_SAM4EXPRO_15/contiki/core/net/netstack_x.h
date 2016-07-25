/**
 * Copyright (c) 2014, Ioannis Glaropoulos
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or
 * other materials provided with the distribution.
 * 
 * 3. Neither the name of the Calipso nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific
 * prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/ 
/*
 * netstack_x.h
 *
\file 
 *   Include file for the Contiki low-layer network stack (NETSTACK_0, NETSTACK_1)
 *
 * Created: 5/2/2014 12:25:16 PM
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 


#ifndef NETSTACK_X_H_
#define NETSTACK_X_H_


#include "contiki-conf.h"
#include "net/netstack.h"

/** \brief Physical interface type */
typedef enum netstack_link_type {
  NETSTACK_NULL = 0,
  NETSTACK_8023,
  NETSTACK_80211,
  NETSTACK_802154,
  NETSTACK_802154_SEC,
  NETSTACK_BLE,
} netstack_link_type_t;

/**
 * In case of multiple interfaces the NETSTACK_NET module is
 * responsible for informing the UIP, about the interface of 
 * the incoming packet. In case of a single interface, it is
 * possible that multiple NETSTACK_MAC modules share a single
 * NETSTACK_NET module, so the following macro ensures that
 * the correct interface type info is passed to
 */

#if (defined UIP_CONF_MULTI_IFACES) && (UIP_CONF_MULTI_IFACES != 0)
#define link_if_in(ll_type)
#else
/* Indicate the link type of an incoming frame */
extern netstack_link_type_t link_if;
#define link_if_in(ll_type) link_if = ll_type
#endif /* UIP_MULTI_IFACES */

/** Netstack_0 [Ethernet] */

#ifndef NETSTACK_0_NETWORK
#ifdef NETSTACK_0_CONF_NETWORK
#define NETSTACK_0_NETWORK NETSTACK_0_CONF_NETWORK
#else /* NETSTACK_0_CONF_NETWORK */
#define NETSTACK_0_NETWORK rime_driver
#endif /* NETSTACK_0_CONF_NETWORK */
#endif /* NETSTACK_0_NETWORK */

#ifndef NETSTACK_0_LLSEC
#ifdef NETSTACK_0_CONF_LLSEC
#define NETSTACK_0_LLSEC NETSTACK_0_CONF_LLSEC
#else /* NETSTACK_0_CONF_LLSEC */
#define NETSTACK_0_LLSEC nullsec_driver
#endif /* NETSTACK_0_CONF_LLSEC */
#endif /* NETSTACK_0_LLSEC */

#ifndef NETSTACK_0_MAC
#ifdef NETSTACK_0_CONF_MAC
#define NETSTACK_0_MAC NETSTACK_0_CONF_MAC
#else /* NETSTACK_0_CONF_MAC */
#define NETSTACK_0_MAC     nullmac_driver
#endif /* NETSTACK_0_CONF_MAC */
#endif /* NETSTACK_0_MAC */

#ifndef NETSTACK_0_FRAMER
#ifdef NETSTACK_0_CONF_FRAMER
#define NETSTACK_0_FRAMER NETSTACK_0_CONF_FRAMER
#else /* NETSTACK_0_CONF_FRAMER */
#define NETSTACK_0_FRAMER   framer_nullmac
#endif /* NETSTACK_0_CONF_FRAMER */
#endif /* NETSTACK_0_FRAMER */

/** Netstack 1 [WLAN] */

#ifndef NETSTACK_1_NETWORK
#ifdef NETSTACK_1_CONF_NETWORK
#define NETSTACK_1_NETWORK NETSTACK_1_CONF_NETWORK
#else /* NETSTACK_1_CONF_NETWORK */
#define NETSTACK_1_NETWORK rime_driver
#endif /* NETSTACK_1_CONF_NETWORK */
#endif /* NETSTACK_1_NETWORK */

#ifndef NETSTACK_1_LLSEC
#ifdef NETSTACK_1_CONF_LLSEC
#define NETSTACK_1_LLSEC NETSTACK_1_CONF_LLSEC
#else /* NETSTACK_1_CONF_LLSEC */
#define NETSTACK_1_LLSEC nullsec_driver
#endif /* NETSTACK_1_CONF_LLSEC */
#endif /* NETSTACK_1_LLSEC */

#ifndef NETSTACK_1_MAC
#ifdef NETSTACK_1_CONF_MAC
#define NETSTACK_1_MAC NETSTACK_1_CONF_MAC
#else /* NETSTACK_1_CONF_MAC */
#define NETSTACK_1_MAC     nullmac_driver
#endif /* NETSTACK_1_CONF_MAC */
#endif /* NETSTACK_1_MAC */

#ifndef NETSTACK_1_RDC
#ifdef NETSTACK_1_CONF_RDC
#define NETSTACK_1_RDC NETSTACK_1_CONF_RDC
#else /* NETSTACK_1_CONF_RDC */
#define NETSTACK_1_RDC		nullrdc_driver
#endif /* NETSTACK_1_CONF_RDC */
#endif /* NETSTACK_1_RDC */

#ifndef NETSTACK_1_FRAMER
#ifdef NETSTACK_1_CONF_FRAMER
#define NETSTACK_1_FRAMER NETSTACK_1_CONF_FRAMER
#else /* NETSTACK_1_CONF_FRAMER */
#define NETSTACK_1_FRAMER   framer_nullmac
#endif /* NETSTACK_1_CONF_FRAMER */
#endif /* NETSTACK_1_FRAMER */

#include "net/mac/mac.h"
#include "net/mac/framer.h"
#include "net/mac/rdc.h"
#include "dev/radio.h"

extern const struct network_driver NETSTACK_0_NETWORK;
extern const struct llsec_driver   NETSTACK_0_LLSEC;
extern const struct mac_driver     NETSTACK_0_MAC;
extern const struct framer         NETSTACK_0_FRAMER;

extern const struct network_driver NETSTACK_1_NETWORK;
extern const struct llsec_driver   NETSTACK_1_LLSEC;
extern const struct mac_driver     NETSTACK_1_MAC;
extern const struct rdc_driver     NETSTACK_1_RDC;
extern const struct framer         NETSTACK_1_FRAMER;

/* TODO 
 *Sanity check; if modules for the networks stacks 
 * are the same, it could lead to potential errors.
 */
 

void netstack_0_init(void);
void netstack_1_init(void);

#endif /* NETSTACK_X_H_ */
