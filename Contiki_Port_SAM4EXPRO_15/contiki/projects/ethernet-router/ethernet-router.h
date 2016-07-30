/*
 * ethernet_router.h
 *
 * Created: 2016-07-29 23:39:45
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 
#include "contiki.h"
#include "process.h"

#ifndef ETHERNET_ROUTER_H_
#define ETHERNET_ROUTER_H_

#define ETHERNET_ROUTER_PREFIX_ETH  0x20, 0x01, 0x11, 0x21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0


#define ETHERNET_ROUTER_PREFIX_LEN  64

PROCESS_NAME(ethernet_router_process);

extern process_event_t ethernet_router_connect_event;

#define NET_PROC_EVENT ethernet_router_connect_event


#endif /* ETHERNET_ROUTER_H_ */