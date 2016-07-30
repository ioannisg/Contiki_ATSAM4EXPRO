/*
 * project_conf.h
 *
 * Created: 2016-07-29 23:40:29
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 


#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#if ETHERNET_ROUTER_SUPPORT

/* Sanity check; Ethernet router needs Ethernet support */
#if !WITH_ETHERNET_SUPPORT
#error "Ethernet router is requested without Ethernet support"
#endif

/* For Ethernet nodes the LINKADDR size is set to 6
 * which is safe, as the LINKADDR0 size is also 6.
 */
#ifdef LINKADDR_CONF_SIZE
#undef LINKADDR_CONF_SIZE
#endif
#define LINKADDR_CONF_SIZE                   6

/* Router needs to route packets */
#ifdef UIP_CONF_ROUTER
#undef  UIP_CONF_ROUTER
#endif
#define UIP_CONF_ROUTER						      1

/* Ethernet routers have a single interface AT THE MOMENT (FIXME) */
#ifdef UIP_CONF_MULTI_IFACES
#if UIP_CONF_MULTI_IFACES	!= 0
#undef UIP_CONF_MULTI_IFACES
#define UIP_CONF_MULTI_IFACES                0
#endif
#else
#define UIP_CONF_MULTI_IFACES                0
#endif /* UIP_CONF_MULTI_IFACES */


/* Do not advertise DNS server in RA packets */
#ifdef UIP_CONF_DS6_RDNSS_INFORMATION
#undef UIP_CONF_DS6_RDNSS_INFORMATION
#endif
#define UIP_CONF_DS6_RDNSS_INFORMATION       0

/* Routing information objects included in RA packets */
#ifdef UIP_CONF_DS6_ROUTE_INFORMATION
#undef UIP_CONF_DS6_ROUTE_INFORMATION
#endif
#define UIP_CONF_DS6_ROUTE_INFORMATION       1

/* Do not advertise default route, to handle only relevant IPV6 traffic */
#ifdef UIP_RA_ADVERTISE_DEFAULT_ROUTE
#undef UIP_RA_ADVERTISE_DEFAULT_ROUTE
#endif
#define UIP_RA_ADVERTISE_DEFAULT_ROUTE       1

/* Send ICMP port unreachable messages */
#define UIP_UDP_SEND_UNREACH_NOPORT          1

/* Send ICMP destination unreachable messages */
#ifdef UIP_ICMP_SEND_UNREACH_ROUTE
#undef UIP_ICMP_SEND_UNREACH_ROUTE
#endif
#define UIP_ICMP_SEND_UNREACH_ROUTE          1

/* We increase the neighbor table size */
#ifdef NBR_TABLE_CONF_MAX_NEIGHBORS
#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#endif
#define NBR_TABLE_CONF_MAX_NEIGHBORS         16

/* Router needs to send Router Advertisements */
#ifdef UIP_CONF_ND6_SEND_RA
#undef UIP_CONF_ND6_SEND_RA
#endif
#define UIP_CONF_ND6_SEND_RA                 1

/* Router processes RAs from other routers */
#ifdef UIP_CONF_PROCESS_RA_ROUTER
#undef UIP_CONF_PROCESS_RA_ROUTER
#endif
#define UIP_CONF_PROCESS_RA_ROUTER           1

#define UIP_CONF_ND6_RETRANS_TIMER		      5000
#define UIP_CONF_ND6_REACHABLE_TIME		      50000

/* Router needs an extended space for prefix information */
#ifdef UIP_CONF_DS6_PREFIX_NBU
#undef UIP_CONF_DS6_PREFIX_NBU
#endif
#define UIP_CONF_DS6_PREFIX_NBU               2

/* And we need more multicast addresses for RIPng and mDNS */
#define UIP_CONF_DS6_MADDR_NBU                3

/* REST chunk size should be large, to accommodate for large routing table payloads */
#define REST_MAX_CHUNK_SIZE                   256

#ifndef CONTIKI_CONF_DEFAULT_HOSTNAME
#define CONTIKI_CONF_DEFAULT_HOSTNAME         "contiki-ethernet-router"
#endif

#endif /* ETHERNET_ROUTER_SUPPORT */

#endif /* PROJECT-CONF_H_ */