/*
 * Copyright (c) 2013, CETIC.
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
 */

/**
 * \file
 *         Header file for RIO support in RA
 * \author
 *         Maciej Wasilak <wasilak@gmail.com>
 *         Maxime Denis <maxime.dn@gmail.com>
 *         6LBR Team <6lbr@cetic.be>
 */
#ifndef UIP_DS6_RIO_H_
#define UIP_DS6_RIO_H_

#include "net/ip/uip.h"

/* Route information list (RFC4191) */
#define UIP_DS6_ROUTE_INFO_NBS 0
#define UIP_CONF_DS6_ROUTE_INFO_NBU 4
#ifndef UIP_CONF_DS6_ROUTE_INFO_NBU
#define UIP_DS6_ROUTE_INFO_NBU 1
#else
#define UIP_DS6_ROUTE_INFO_NBU UIP_CONF_DS6_ROUTE_INFO_NBU
#endif
#define UIP_DS6_ROUTE_INFO_NB UIP_DS6_ROUTE_INFO_NBS + UIP_DS6_ROUTE_INFO_NBU

#define UIP_ND6_OPT_ROUTE_INFO          24

#define UIP_ND6_RA_FLAG_PRF_HIGH        0x18
#define UIP_ND6_RA_FLAG_PRF_LOW         0x08

#define UIP_DEFAULT_METRIC              10

/** \brief An entry in the specific routes table (RFC4191) */
typedef struct uip_ds6_route_info {
  uint8_t isused;
  uip_ipaddr_t ipaddr;
  uint8_t length;
  uint8_t flags;
  uint32_t lifetime;
} uip_ds6_route_info_t;

/** \brief ND option route information */
typedef struct uip_nd6_opt_route_info {
  uint8_t type;
  uint8_t len;
  uint8_t preflen;
  uint8_t flagsreserved;
  uint32_t rlifetime;
  uip_ipaddr_t prefix;
} uip_nd6_opt_route_info;

extern uip_ds6_route_info_t uip_ds6_route_info_list[UIP_DS6_ROUTE_INFO_NB];

uip_ds6_route_info_t *uip_ds6_route_info_add(uip_ipaddr_t * ipaddr,
                                             uint8_t ipaddrlen, uint8_t flags,
                                             unsigned long rlifetime);
void uip_ds6_route_info_rm(uip_ds6_route_info_t * rtinfo);
uip_ds6_route_info_t *uip_ds6_route_info_lookup(uip_ipaddr_t * ipaddr,
                                                uint8_t ipaddrlen);
/** \brief Callback called in RA input to deal with RIO */
void uip_ds6_route_info_callback(uip_nd6_opt_route_info *, uip_ip6addr_t *);
#endif /* UIP6-RIO_H_ */