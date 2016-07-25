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
 * \file
 * Contiki Ethernet driver header file
 *
 * Created: 5/1/2014 9:23:49 PM
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se> 
 *
 */ 
#ifndef ETHERNET_H_
#define ETHERNET_H_

#include "net/linkaddr6.h"
#include "net/mac/mac.h"

#ifndef ETHERNET_CONF_MAX_FRAME_LEN
#warning "Maximum ethernet frame length should be defined in platform-conf.h. Using default value."
#define ETHERNET_MAX_FRAME_LEN  1536
#else
#define ETHERNET_MAX_FRAME_LEN ETHERNET_CONF_MAX_FRAME_LEN
#endif /* ETHERNET_CONF_MAX_FRAME__LEN */

/**
 * Structure for statistics collection of an Ethernet driver
 */
typedef struct {
  uint32_t tx;                    /* Transmitted frames counter */
  uint32_t rx;                    /* Received frames counter */
  uint32_t tx_err;                /* Transmit errors counter */
  uint32_t rx_err_no_buf;         /* Receive errors counter for no buffer availability */
  uint32_t rx_err_overrun;        /* Receive errors counter for RX over-run*/
  uint32_t reset_count;           /* Reset counter */
} eth_stats_t;

/** List of packets to be sent by Ethernet driver */
struct eth_pkt_buf {
  struct eth_pkt_buf *next;
  uint32_t pkt_len;
  uint8_t pkt_data[ETHERNET_MAX_FRAME_LEN];
};
typedef struct eth_pkt_buf eth_pkt_buf_t;

/* Generic Ethernet-driver TX return values */
typedef enum {
  /**< The Ethernet-driver frame transmission was OK. */
  ETHERNET_TX_OK,

  /** < The Ethernet-driver frame transmission failed due to the link
   being inactive */
  ETHERNET_TX_ERR_OFFLINE,
  
  /**< The Ethernet-driver frame transmission was unsuccessful due to
    errors in transmission that are not recognized */
  ETHERNET_TX_ERR_COLLISION,
        
  /**< The Ethernet-driver frame transmission could not be performed
   due to a fatal error */
  ETHERNET_TX_ERR_FATAL,
        
  /**< The Ethernet-driver frame transmission could not be performed
   as the driver timed-out */
  ETHERNET_TX_ERR_TIMEOUT,
        
  /**< The Ethernet-driver frame transmission could not be performed
   due to reaching the maximum number of pending frames */
  ETHERNET_TX_ERR_FULL,   
} ethernet_tx_status_t;

struct eth_driver {
        
  const char *name;
        
  /** Initialize the Ethernet device */
  void (*init)(void);
        
  /** Reset the Ethernet device */
  void (*reset)(void);
                
  /** Set the device MAC address */		 
  void (*set_macaddr)(linkaddr6_t*);
			
  /** Read the device MAC address */
  linkaddr6_t* (*get_macaddr)(void);
					 
  /** Transmit an Ethernet packet or add it to the driver outgoing queue */
  void (*tx_packet)(mac_callback_t sent_callback, void *ptr);
  
  /** Check whether the interface has been initialized */
  uint8_t (*is_initialized)(void);
        
  /** Check whether the Ethernet link is active */
  uint8_t (*is_up)(void);
  
  /** Switch-on Ethernet interface */
  uint8_t (*on)(void);
  
  /** Switch-off Ethernet interface */
  uint8_t (*off)(uint8_t keep_resources);
};


#endif /* ETHERNET_H_ */
