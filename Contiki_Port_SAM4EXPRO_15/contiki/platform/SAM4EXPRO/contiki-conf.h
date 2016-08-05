/*
 * contiki_conf.h
 *
 * Created: 2015-10-25 00:11:26
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */


#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

#include "platform-conf.h"

#ifdef AUTOSTART_ENABLE
#define AUTOSTART_ENABLE                                 1
#endif /* AUTOSTART_ENABLE */

#ifndef WITH_SERIAL_DEBUG
#define WITH_SERIAL_DEBUG                                0
#else
#define WITH_SERIAL_DEBUG                                1
#endif


/* Radio and 802.15.4 params */
/* 802.15.4 radio channel */
#define RF_CHANNEL													26
/* 802.15.4 PAN ID */
#define IEEE802154_CONF_PANID										0xabcd
/* Use EID 64, enable hardware autoack and address filtering */
#define ST_CONF_RADIO_AUTOACK										1
/* Number of buffers for incoming frames */
#define RADIO_RXBUFS													2
/* Set to 0 for non ethernet links */
#define UIP_CONF_LLH_LEN											0

/* RDC params */
/* TX routine passes the cca/ack result in the return parameter */
#define RDC_CONF_HARDWARE_ACK										1
/* TX routine does automatic cca and optional backoff */
#define RDC_CONF_HARDWARE_CSMA									0
/* RDC debug with LED */
#define RDC_CONF_DEBUG_LED											1
/* Channel check rate (per second) */
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE					8
/* Use ACK for optimization (LPP, XMAC) */
#define WITH_ACK_OPTIMIZATION										0

/* ContikiMAC config */
#define CONTIKIMAC_CONF_COMPOWER									1
#define CONTIKIMAC_CONF_BROADCAST_RATE_LIMIT					0
#define CONTIKIMAC_CONF_ANNOUNCEMENTS							0
#define CONTIKIMAC_CONF_WITH_CONTIKIMAC_HEADER           0

/* CXMAC config */
#define CXMAC_CONF_ANNOUNCEMENTS									0
#define CXMAC_CONF_COMPOWER										1

/* XMAC config */
#define XMAC_CONF_ANNOUNCEMENTS									0
#define XMAC_CONF_COMPOWER											1

/* Other (RAM saving) */
#define ENERGEST_CONF_ON											0
#define QUEUEBUF_CONF_NUM											8
#define QUEUEBUF_CONF_REF_NUM										0
#define NBR_TABLE_CONF_MAX_NEIGHBORS							4
#define UIP_CONF_DS6_ROUTE_NBU									4
#define RPL_CONF_MAX_PARENTS_PER_DAG							4
#define RPL_CONF_MAX_INSTANCES									1
#define RPL_CONF_MAX_DAG_PER_INSTANCE							1
#define PROCESS_CONF_NUMEVENTS									16
/* The process names are not used to save RAM */
#define PROCESS_CONF_NO_PROCESS_NAMES                    1
#define CC_CONF_NO_VA_ARGS											0
#define PACKETBUF_CONF_ATTRS_INLINE								1
#define RIMESTATS_CONF_ENABLED									0
#define CC_CONF_UNSIGNED_CHAR_BUGS								0
#define CC_CONF_DOUBLE_HASH										0
#define CONTIKI_CONF_SETTINGS_MANAGER							0
#define PROCESS_CONF_STATS											0
#define MMEM_CONF_SIZE												256// We do not use this memory


/* Netstack_0 & IEEE802.3 configuration [Interface 0] */
#if WITH_ETHERNET_SUPPORT
#define ETHERNET_CONF_STACK                              ETHERNET_DEV_DRIVER
#define NETSTACK_0_CONF_MAC                              ieee8023_mac_driver
#define NETSTACK_0_CONF_FRAMER                           ieee8023_framer

/* Handling of 802.3 frames requires a larger packet buffer */
#ifdef PACKETBUF_CONF_SIZE
#undef PACKETBUF_CONF_SIZE
#endif
#define PACKETBUF_CONF_SIZE                              ETHERNET_CONF_MAX_FRAME_LEN
#define PACKETBUF_CONF_WITH_PACKET_TYPE                  1

/* Link addresses sizes */
#define LINKADDR_CONF_SIZE                               6
#define LINKADDR0_CONF_SIZE                              6

#endif /* WITH_ETHERNET_SUPPORT */

/* ------------------ IPv6 configuration for SAM4EXPRO --------------------- */

/* The following flags shall be in the Makefile if we use the default Contiki
 * build system. For Atmel Studio, we define them in contiki-conf.h.
 */
#define NETSTACK_CONF_WITH_RIME                          1 /* For compilation only */
#define NETSTACK_CONF_WITH_IP64                          0
#define NETSTACK_CONF_WITH_RIPNG                         0
#define WITH_COAP                                        7

#if NETSTACK_CONF_WITH_IPV6

/* TCP if CoAP is needed. Set in Makefile for CoAP support. */
#if (WITH_COAP==7) || (WITH_COAP==6) || (WITH_COAP==3)
#define UIP_CONF_TCP													0
#define REST coap_rest_implementation
#else
#define UIP_CONF_TCP													1
#endif /* WITH_COAP */

#define UIP_CONF_UDP                							1
#define UIP_CONF_ICMP6												1

/* Default is single interface: override on netop-conf.h */
#define UIP_CONF_MULTI_IFACES										0
/* Default is single radio: override on netop-conf.h */
#define NETSTACK_CONF_WITH_DUAL_RADIO                    0
/* Default is not sending RIO in RAs */
#define UIP_CONF_DS6_ROUTE_INFORMATION                   0

/* Default forwarding and router configuration */
#define UIP_CONF_ROUTER                                  1
#define UIP_CONF_ND6_SEND_NA                             1
#define UIP_CONF_ND6_SEND_RA                             0
#define UIP_RA_ADVERTISE_DEFAULT_ROUTE                   0
#define UIP_CONF_PROCESS_RA_ROUTER                       0
#define UIP_CONF_DS6_RDNSS_INFORMATION                   0
#define UIP_ICMP_SEND_UNREACH_ROUTE                      0
#define UIP_CONF_ND6_MAX_PREFIXES                        2
#define UIP_CONF_ND6_MAX_DEFROUTERS                      1

/* Default IPv6 configuration: override on netop-conf.h */
#define UIP_CONF_IPV6												1
#define UIP_CONF_IPV6_QUEUE_PKT									1//0 We have enough memory
#define UIP_CONF_IPV6_CHECKS										1
#define UIP_CONF_IPV6_REASSEMBLY									0

#define UIP_CONF_DS6_ADDR_NBU										2

#define UIP_CONF_IP_FORWARD										0
#define UIP_CONF_MAX_CONNECTIONS									4
#define UIP_CONF_MAX_LISTENPORTS									8
#define UIP_CONF_UDP_CONNS											4
#define TCPIP_CONF_ANNOTATE_TRANSMISSIONS						0

/* Default RPL configuration is OFF */
#define UIP_CONF_IPV6_RPL                                0

#define UIP_CONF_BUFFER_SIZE                             1280

/* Common IPv6 Configuration for Ethernet and WiFi */
#if (((WITH_WIFI_SUPPORT) || (WITH_ETHERNET_SUPPORT)) && (!(WITH_ZIGBEE_SUPPORT)))
#define UIP_CONF_LL_802154											0
#define UIP_CONF_LL_80211											1
#else
#define UIP_CONF_LL_802154                               1
#define UIP_CONF_LL_80211                                0
#endif

/* IPv6 Configuration for each physical interface: 802.3, 802.11, or 802.15.4 */
#if WITH_ETHERNET_SUPPORT
#define NETSTACK_0_CONF_NETWORK									ieee8023_net_driver
#endif

#if WITH_WIFI_SUPPORT
#define NETSTACK_1_CONF_NETWORK									ieee80211_net_driver
#endif

#if WITH_ZIGBEE_SUPPORT
#define NETSTACK_CONF_NETWORK										sicslowpan_driver
/* Specify a minimum packet size for 6lowpan compression to be
   enabled. This is needed for ContikiMAC, which needs packets to be
   larger than a specified size, if no ContikiMAC header should be
   used. */
#define SICSLOWPAN_CONF_COMPRESSION_THRESHOLD            63
#define SICSLOWPAN_CONF_MAC_MAX_PAYLOAD                  111
#define SICSLOWPAN_CONF_COMPRESSION_IPV6                 0
#define SICSLOWPAN_CONF_COMPRESSION_HC1                  1
#define SICSLOWPAN_CONF_COMPRESSION_HC01                 2
#define SICSLOWPAN_CONF_COMPRESSION                      SICSLOWPAN_COMPRESSION_HC06
#ifndef SICSLOWPAN_CONF_FRAG
#define SICSLOWPAN_CONF_FRAG                             1
#define SICSLOWPAN_CONF_MAXAGE                           8
#endif /* SICSLOWPAN_CONF_FRAG */
#define SICSLOWPAN_CONF_CONVENTIONAL_MAC	               1
#define SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS                2
#ifndef SICSLOWPAN_CONF_ADDR_CONTEXT_0
#define SICSLOWPAN_CONF_ADDR_CONTEXT_0 { \
	addr_contexts[0].prefix[0] = 0xaa; \
	addr_contexts[0].prefix[1] = 0xaa; \
}
#endif
#ifndef SICSLOWPAN_CONF_MAX_MAC_TRANSMISSIONS
#define SICSLOWPAN_CONF_MAX_MAC_TRANSMISSIONS            5
#endif /* SICSLOWPAN_CONF_MAX_MAC_TRANSMISSIONS */
#endif  /* WITH_ZIGBEE_SUPORT */

#else /* NETSTACK_CONF_WITH_IPV6 */

/* Network setup for non-IPv6 (rime). */
#define NETSTACK_CONF_NETWORK										rime_driver

#define UIP_CONF_TCP													0
#define UIP_CONF_UDP													0
#define UIP_CONF_ICMP6                                   0
#define UIP_CONF_IPV6                                    0

#endif /* NETSTACK_CONF_WITH_IPV6 */

/* ------------------------  Network Operation Mode  ----------------------- */
#include "../../contiki/projects/network-conf.h"

/* If there is particular network configuration, include it */
#ifdef PROJECT_NET_CONF_H
#include PROJECT_NET_CONF_H
#endif

/* ----- Macros for atomic operations -------*/
#define INTERRUPTS_DISABLE()					               cpu_irq_enter_critical()
#define INTERRUPTS_ENABLE()					               cpu_irq_leave_critical()
#define INTERRUPT_DISABLE(irq_id)			               NVIC_DisableIRQ(irq_id)
#define INTERRUPT_ENABLE(irq_id)				               NVIC_EnableIRQ(irq_id)

#if COFFEE == 1
#define CONTIKI_FLASH_BANKS_NUM                          4
#define CONTIKI_FLASH_SIZE_BANK0                         (IFLASH_SIZE)/(CONTIKI_FLASH_BANKS_NUM)
#define CONTIKI_FLASH_SIZE_BANK1                         (IFLASH_SIZE)/(CONTIKI_FLASH_BANKS_NUM)
#define CONTIKI_FLASH_SIZE_BANK2                         (IFLASH_SIZE)/(CONTIKI_FLASH_BANKS_NUM)
#define CONTIKI_FLASH_SIZE_BANK3                         (IFLASH_SIZE)/(CONTIKI_FLASH_BANKS_NUM)

#define CONTIKI_FLASH_ADDR_BANK0                         IFLASH_ADDR
#define CONTIKI_FLASH_ADDR_BANK1                         CONTIKI_FLASH_ADDR_BANK0 + CONTIKI_FLASH_SIZE_BANK0
#define CONTIKI_FLASH_ADDR_BANK2                         CONTIKI_FLASH_ADDR_BANK1 + CONTIKI_FLASH_SIZE_BANK1
#define CONTIKI_FLASH_ADDR_BANK3                         CONTIKI_FLASH_ADDR_BANK2 + CONTIKI_FLASH_SIZE_BANK2


#ifndef COFFEE_ADDRESS
#define COFFEE_ADDRESS                                   CONTIKI_FLASH_ADDR_BANK2
#endif /* COFFEE_ADDRESS */
#endif /* COFFEE == 1 */

/* Include project-specific configuration, if present */
#ifdef PROJECT_CONF_H
#include PROJECT_CONF_H
#endif /* PROJECT_CONF_H */

#endif /* CONTIKI-CONF_H_ */
