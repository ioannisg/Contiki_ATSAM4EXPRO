/*
 * project_conf.h
 *
 * Created: 2016-06-14 20:19:23
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 


#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_


#if ETHERNET_NODE_SUPPORT

/* Sanity check; Ethernet node needs Ethernet support */
#if !WITH_ETHERNET_SUPPORT
#error "Ethernet node is requested without Ethernet support"
#endif

/* For Ethernet nodes the LINKADDR size is set to 6
 * which is safe, as the LINKADDR0 size is also 6.
 */
#ifdef LINKADDR_CONF_SIZE
#undef LINKADDR_CONF_SIZE
#endif
#define LINKADDR_CONF_SIZE                                6


/* Ethernet nodes do not route packets */
#ifdef UIP_CONF_ROUTER
#if UIP_CONF_ROUTER
#undef  UIP_CONF_ROUTER
#define UIP_CONF_ROUTER                                   0
#endif
#else
#define UIP_CONF_ROUTER                                   0
#endif

/* Ethernet nodes have a single interface */
#ifdef UIP_CONF_MULTI_IFACES
#if UIP_CONF_MULTI_IFACES	!= 0
#undef UIP_CONF_MULTI_IFACES	
#define UIP_CONF_MULTI_IFACES                             0
#endif
#else
#define UIP_CONF_MULTI_IFACES                             0
#endif /* UIP_CONF_MULTI_IFACES */

#endif /* ETHERNET_NODE_SUPPORT */
#endif /* PROJECT_CONF_H_ */