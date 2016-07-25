/*
 * ieee8023_net.c
 *
 * Created: 5/2/2014 1:04:56 PM
 *  Author: Ioannis Glaropoulos	
 */ 

#include "net/linkaddr6.h"
#include "net/netstack_x.h"

#include "compiler.h"
#include "mini_ip.h"
#include "packetbuf.h"

#include "net/ip/uip.h"
#include "net/ip/tcpip.h"
#if (defined UIP_MULTI_IFACES) && (UIP_MULTI_IFACES != 0)
#include "net/ipv6/uip-ds6.h"
#endif

#include "ipv6/uip-ds6-nbr.h"
#include "ipv4/uip_arp.h"

#include "process.h"

#if WITH_ETHERNET_SUPPORT
#ifdef NETSTACK_CONF_WITH_IPV6

#define UIP_IP_BUF				((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#if DEBUG == 0
#if WITH_SERIAL_DEBUG
#undef PRINTF
#undef PRINT6ADDR
#define PRINTF(...) if (net_debug.mac) { \
	printf(__VA_ARGS__); \
}
#define PRINT6ADDR(addr) if (net_debug.mac) { \
	uip_debug_ipaddr_print(addr); \
}
#endif
#endif

#if (defined UIP_MULTI_IFACES) && (UIP_MULTI_IFACES != 0)
static uip_ds6_iface_t *uip_iface = NULL; /** \brief a pointer to the corresponding DS6 interface */
#else
netstack_link_type_t link_if;
#endif /* UIP_MULTI_IFACES */

/*---------------------------------------------------------------------------*/
static void
packet_sent(void* ptr, int status, int retransmissions) {

  UNUSED(ptr);
  
  if(!status) {
    PRINTF("ieee8023_net: Packet was sent/scheduled successfully.\n");
	 
  } else {
    if (retransmissions == 0) {
	   PRINTF("ieee8023_net: pkt-not-attempted\n");
    } else {
      PRINTF("ieee8023_net: not-successful\n");
    }
  }
  /* Inform UIP, RPL */
#if ! UIP_MULTI_IFACES
  uip_ds6_link_neighbor_callback(status, retransmissions);
#else
  uip_ds6_link_neighbor_callback(uip_iface, status, retransmissions);
#endif
}
/*---------------------------------------------------------------------------*/
/** \brief Process a received 802.11/802.3 packet.
 *  \param r The MAC layer
 *
 * The 802.x packet payload is put in the "packetbuf" by the underlying
 * MAC. It is actually an IP packet. The IP packet must be complete and,
 * thus, it is copied to the "uip_buf" and the IP layer is called. This 
 * function does nothing so crucial unless multiple interfaces are used
 */
static void
input(void)
{
  PRINTF("ieee8023_net: recv (%u)\n", packetbuf_datalen());
#if UIP_MULTI_IFACES
  if (unlikely(uip_len != 0 || uip_in_if != NULL || uip_out_if != NULL)) {
    printf("ieee8023_net: non-empty-uip; drop. %u %u %u\n",
      uip_len != 0,
      uip_in_if != NULL,
      uip_out_if != NULL);
    return;
  }
#else /* UIP_MULTI_IFACES */
  if (uip_len != 0) {
    PRINTF("ieee8023_net: non-empty-uip; drop.\n");
    return;  
  }
#endif /* UIP_MULTI_IFACES */
  
  memcpy((uint8_t *)UIP_IP_BUF, (uint8_t*)(packetbuf_dataptr()),
    packetbuf_datalen());
	 
  uip_len = packetbuf_datalen();
#if UIP_MULTI_IFACES
  uip_in_if = uip_iface;
#endif /* UIP_MULTI_IFACES */ 
  
#if DEBUG
  int i;
  uint8_t *pkt = packetbuf_dataptr();
  PRINTF("ieee8023-net-IN[%u]:",uip_len);
  for (i=0; i<packetbuf_datalen(); i++)
    PRINTF("%02x ", pkt[i]);
	 
  PRINTF("\n");
#endif

  /* Deliver the packet to the "uip" layer. */
  tcpip_input();
}
/*---------------------------------------------------------------------------*/
/** \brief Take an IP packet and format it to be sent on an 802.3
 *  network using the underlying WiFi implementation.
 *  \param localdest The MAC address of the destination
 *
 *  The IP packet is initially in uip_buf. The resulting packet is 
 *  put in the "packetbuf", and delivered to the 802.3 MAC module.
 */
static uint8_t
output(const uip_lladdr_t *localdest)
{
  PRINTF("ieee8023-net: OUT[%u]\n", uip_len);
  if (unlikely(uip_len == 0)) {

    PRINTF("ieee8023_net: empty/corrupted UIP packet\n");
    return 0;
  }
#if DEBUG
  PRINTF("UIP [%u]: ",uip_len);
  for(int i=0; i<uip_len; i++) {
    PRINTF("%02x ", uip_buf[i]);
  }
  PRINTF("\n");
#endif
  
  packetbuf_clear();
  
  /* Copy the UIP payload to the packet buffer */
  if (unlikely(!packetbuf_copyfrom(uip_buf, uip_len) == uip_len)) {
    PRINTF("ieee8023_net: too-large-pkt (%u)\n",uip_len);
    return 0;
  }
  /* Packet length */
  packetbuf_set_datalen(uip_len);
  
  /* Source MAC address. Cast to linkaddr_t */
  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, (linkaddr_t*)&linkaddr6_node_addr);

  /* Destination MAC address */
  if (localdest != NULL) {
	  
    packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER,
	   (linkaddr_t*)&localdest->addr[0]);
    PRINTF("ieee8023_net: tx (%u) to (%02x)\n", uip_len, localdest->addr[0]);
	 
  } else {
    /* IPv6 does not use broadcast address */	  
    PRINTF("ieee8023_NET: broadcast\n");
	 linkaddr6_t eth_mcast_addr;
	 eth_mcast_addr.u8[0] = 0x33;
	 eth_mcast_addr.u8[1] = 0x33;
	 eth_mcast_addr.u8[2] = UIP_IP_BUF->destipaddr.u8[12];
	 eth_mcast_addr.u8[3] = UIP_IP_BUF->destipaddr.u8[13];
	 eth_mcast_addr.u8[4] = UIP_IP_BUF->destipaddr.u8[14];
	 eth_mcast_addr.u8[5] = UIP_IP_BUF->destipaddr.u8[15];
	 packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, (linkaddr_t*)&eth_mcast_addr);
  }
  
  /* Maybe redundant since DATA type is zero value? */
  packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,
    PACKETBUF_ATTR_PACKET_TYPE_DATA);
  
  /* Set the protocol flag */
#if NETSTACK_CONF_WITH_IPV6
  packetbuf_set_attr(PACKETBUF_ATTR_ETH_PROTO, ETH_PROT_IPV6);
#else /* Redundant for now */
  packetbuf_set_attr(PACKETBUF_ATTR_ETH_PROTO, ETH_PROT_IP);
#endif
  /* Send the packet down to the MAC */
  NETSTACK_0_MAC.send(packet_sent, NULL);
  
  return 1;
}
/*---------------------------------------------------------------------------*/
static void
init(void) 
{  
  PRINTF("IEEE8023_NET: init\n");
  /*
   * Register a new network interface at UIP
   * supplying the Link Layer address of the
   * physical interface, the output function
   * for TCP/IP and the type of the physical
   * interface.
   */
#if ! UIP_MULTI_IFACES
  linkaddr_copy(&linkaddr_node_addr, (linkaddr_t*)&linkaddr6_node_addr);
 
  /* This is the only interface so we must set up the
   * link address for UIP4, particularly, for the ARP.
   */
#pragma message("LINK address set by Ethernet")
  uip_lladdr_t eth_addr;
  memcpy(eth_addr.addr, &linkaddr6_node_addr, UIP_LLADDR_LEN);
  uip_setethaddr(eth_addr);  
#endif /* ! UIP_MULTI_IFACES */
}
/*---------------------------------------------------------------------------*/
static void
connect_event(uint8_t up)
{
  if (up) {
    /* Interface is up. Start UIP6 if not running yet. */
    if (!process_is_running(&tcpip_process)) {
      PRINTF("IEEE8023_net: starting-uip-stack\n");
      process_start(&tcpip_process, NULL);
    }
#if ! UIP_MULTI_IFACES
    tcpip_set_outputfunc(output);
#else /* UIP_MULTI_IFACES */
    /* Register the interface with UIP-DS6 */
    if ((uip_iface = uip_ds6_register_net_iface(NETSTACK_8023, 
      (linkaddr_t* )&linkaddr6_node_addr, output)) == NULL) {
      PRINTF("ieee8023_net: interface registration failed\n");
      goto _exit;
    }
    /* Inform TCP/IP stack so it can start the interface */
    process_post(&tcpip_process, uip_ds6_iface_event, uip_iface);
#endif /* UIP_MULTI_IFACES */
  } else {
    /* Interface is down. */
#if ! UIP_MULTI_IFACES
    tcpip_set_outputfunc(NULL);
    if (!process_is_running(&tcpip_process)) {
      process_exit(&tcpip_process);
    }
#else
    //uip_iface = uip_ds6_deregister_net_iface(NETSTACK_8023, &linkaddr0_node_addr,
      //output);
#endif /* ! UIP_MULTI_IFACES */
  }
_exit:
  return;
}
/*---------------------------------------------------------------------------*/
const struct network_driver ieee8023_net_driver = {
  (char*)"ieee8023_net",
  init,
  input,
  connect_event,
};
/*---------------------------------------------------------------------------*/
#endif /* WITH_ETHERNET_SUPPORT */
#endif /* NETSTACK_CONF_WITH_IPV6 */ 