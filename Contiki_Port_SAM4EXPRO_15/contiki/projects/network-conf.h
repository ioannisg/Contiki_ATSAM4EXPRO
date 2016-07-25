/*
 * network_conf.h
 *
 * Created: 2016-06-14 19:31:20
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 


#ifndef NETWORK_CONF_H_
#define NETWORK_CONF_H_


/* NET_PROC: the network process for the selected network operation mode,
 *           initialized when the corresponding physical interface sends
 *           a "connected" event up to the UIP stack. Initialization is
 *           done by the tcpip process.  
 */


/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	*/
/*              Network Operation Modes							   */
/* -----------------------------------------------------------	*/

/* Note:
 * The operation node shall be defined in the Makefile.
 * This is the generic Makefile (for Atmel Studio), or
 * the project Makefile, when migrating to the default
 * Contiki build environment.
 */
#ifdef CONTIKI_WITH_ETH_NODE_SUPPORT
#define ETHERNET_NODE_SUPPORT                                  1
#define PROJECT_NET_CONF_H "projects/ethernet-node/project-conf.h"
#define NET_PROC_H "projects/ethernet-node/ethernet-node.h"
#define NET_PROC ethernet_node_process
#else
#define ETHERNET_NODE_SUPPORT                                  0
#endif /* CONTIKI_WITH_ETH_NODE_SUPPORT */



#endif /* NETWORK_CONF_H_ */