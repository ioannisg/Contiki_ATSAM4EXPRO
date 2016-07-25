/* Copyright (c) 2015, Ioannis Glaropoulos
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

/*
 * linkaddr6.h
 *
 * Created: 2014-06-28 01:19:18
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */

/**
 * \addtogroup rime
 * @{
 */

/**
 * \defgroup linkaddr Ethernet addresses, WLAN addresses
 * @{
 *
 * The linkaddr module is an abstract representation of addresses in
 * Rime.
 *
 */
#ifndef LINKADDR6_H_
#define LINKADDR6_H_

#include "contiki-conf.h"

#ifdef LINKADDR6_CONF_SIZE
#define LINKADDR6_SIZE LINKADDR6_CONF_SIZE
#else /* LINKADDR0_SIZE */
#define LINKADDR6_SIZE 6
#endif /* LINKADDR0_SIZE */

#ifdef LINKADDR1_CONF_SIZE
#define LINKADDR1_SIZE LINKADDR1_CONF_SIZE
#else /* LINKADDR1_SIZE */
#define LINKADDR1_SIZE 6
#endif /* LINKADDR1_SIZE */

typedef union {
  unsigned char u8[LINKADDR6_SIZE];
} linkaddr6_t;

typedef union {
	unsigned char u8[LINKADDR1_SIZE];
} linkaddr1_t;

/**
 * \brief      Copy a Rime address
 * \param dest The destination
 * \param from The source
 *
 *             This function copies a Rime address from one location
 *             to another.
 *
 */
void linkaddr6_copy(linkaddr6_t *dest, const linkaddr6_t *from);
void linkaddr1_copy(linkaddr1_t *dest, const linkaddr1_t *from);

/**
 * \brief      Compare two Rime addresses
 * \param addr1 The first address
 * \param addr2 The second address
 * \return     Non-zero if the addresses are the same, zero if they are different
 *
 *             This function compares two Rime addresses and returns
 *             the result of the comparison. The function acts like
 *             the '==' operator and returns non-zero if the addresses
 *             are the same, and zero if the addresses are different.
 *
 */
int linkaddr6_cmp(const linkaddr6_t *addr1, const linkaddr6_t *addr2);
int linkaddr1_cmp(const linkaddr1_t *addr1, const linkaddr1_t *addr2);


/**
 * \brief      Set the address of the current node
 * \param addr The address
 *
 *             This function sets the Rime address of the node.
 *
 */
void linkaddr0_set_node_addr(linkaddr6_t *addr);
void linkaddr1_set_node_addr(linkaddr1_t *addr);

/**
 * \brief      The Rime address of the node
 *
 *             This variable contains the Rime address of the
 *             node. This variable should not be changed directly;
 *             rather, the linkaddr_set_node_addr() function should be
 *             used.
 *
 */
extern linkaddr6_t linkaddr6_node_addr;
extern linkaddr1_t linkaddr1_node_addr;

/**
 * \brief      The null Rime address
 *
 *             This variable contains the null Rime address. The null
 *             address is used in route tables to indicate that the
 *             table entry is unused. Nodes with no configured address
 *             has the null address. Nodes with their node address set
 *             to the null address will have problems communicating
 *             with other nodes.
 *
 */
extern const linkaddr6_t linkaddr6_null;
extern const linkaddr1_t linkaddr1_null;

#endif /* LINKADDR60_H_ */
/** @} */
/** @} */