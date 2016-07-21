/*
 * contiki_conf.h
 *
 * Created: 2015-10-25 00:11:26
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */


#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

#include "platform-conf.h"
#include "dev/ethernet.h"

#ifdef AUTOSTART_ENABLE
#define AUTOSTART_ENABLE                                 1
#endif /* AUTOSTART_ENABLE */

#ifndef WITH_SERIAL_DEBUG
#define WITH_SERIAL_DEBUG                                0
#else
#define WITH_SERIAL_DEBUG                                1
#endif



#define PROCESS_CONF_NO_PROCESS_NAMES                    0

/* Netstack_0 & IEEE802.3 configuration [Interface 0] */
#ifdef ETHERNETSTACK_CONF_WITH_ETHERNET
#define WITH_ETHERNET_SUPPORT                            1
#define ETHERNET_CONF_STACK                              ETHERNET_DEV_DRIVER
#define NETSTACK_0_CONF_MAC                              ieee8023_mac_driver
#define NETSTACK_0_CONF_FRAMER                           ieee8023_framer

/* Handling of 802.3 frames requires a larger packet buffer */
#ifdef PACKETBUF_CONF_SIZE
#undef PACKETBUF_CONF_SIZE
#endif
#define PACKETBUF_CONF_SIZE                              ETHERNET_MAX_FRAME_LEN
#define PACKETBUF_CONF_WITH_PACKET_TYPE                  1
#endif /* ETHERNETSTACK_CONF_WITH_ETHERNET */

#define NETSTACK_CONF_WITH_RIME                          1 /* For compile-reasons only */


/* ------------------------  Network Operation Mode  ----------------------- */
#include "../../contiki/projects/network-conf.h"

/* If there is particular network configuration, include it */
#ifdef PROJECT_NET_CONF_H
#include PROJECT_NET_CONF_H
#endif

/* Include project-specific configuration, if present */
#ifdef PROJECT_CONF_H
#include PROJECT_CONF_H
#endif /* PROJECT_CONF_H */

#endif /* CONTIKI-CONF_H_ */
