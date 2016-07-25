/*
 * ieee8023_mac.c
 *
 * Created: 5/2/2014 5:09:42 PM
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 

#include "net/mac/8023/ieee8023_mac.h"
#include "net/netstack_x.h"
#include "dev/ethernetstack.h"
#include "net/packetbuf.h"
#include "net/mac/mac-sequence.h"
#include "mini_ip.h"

#if WITH_ETHERNET_SUPPORT

#if NETSTACK_CONF_WITH_IP64
#include "ip64.h"
#endif

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

/* 
 * Duplicate frame detection is not featured in 802.3 
 * This option enables future support in Ethernet.
 */
#define ETH_SUPPORT_DUPLICATE_DETECTION 0


/*---------------------------------------------------------------------------*/
static void
send_packet(mac_callback_t sent, void *ptr)
{
  UNUSED(ptr);
  PRINTF("IEEE8023_mac: tx\n");
  if (NETSTACK_0_FRAMER.create() < 0) {
    PRINTF("IEEE8023_MAC: framer-failed-to-create\n");
    mac_call_sent_callback(sent, NULL, MAC_TX_ERR_FATAL, 0);
    return;
  }
  //switch(ETHERNET_STACK.tx_packet(NULL, NULL)) {//FIXME XXX
  switch(1) {
  case ETHERNET_TX_OK:
    mac_call_sent_callback(sent, NULL, MAC_TX_OK, 1);
    break;
  case ETHERNET_TX_ERR_FATAL:
  case ETHERNET_TX_ERR_OFFLINE:
  case ETHERNET_TX_ERR_FULL:
    mac_call_sent_callback(sent, NULL, MAC_TX_ERR_FATAL, 0);
    break;
  case ETHERNET_TX_ERR_COLLISION:
  case ETHERNET_TX_ERR_TIMEOUT:
    mac_call_sent_callback(sent, NULL, MAC_TX_ERR, 1);
    break;
  default:
    PRINTF("ieee8023_mac: unrecognized ETH return status\n");
    break;
  }
}
/*---------------------------------------------------------------------------*/
static void
packet_input(void)
{
#if NETSTACK_CONF_WITH_IP64
  /* If NAT64 is enabled, IPv4 & ARP packets are sent to the IP64 module */
  ethernet_header_t *hdr = (ethernet_header_t *)packetbuf_hdrptr();
  if (hdr->et_protlen == UIP_HTONS(ETH_PROT_IP) ||
    hdr->et_protlen == UIP_HTONS(ETH_PROT_ARP)) {
    IP64_INPUT(packetbuf_hdrptr(), packetbuf_totlen());
    return;
  }
#endif /* NETSTACK_CONF_WITH_IP64 */
  if(NETSTACK_0_FRAMER.parse() < 0) {
    PRINTF("ieee8023_mac: failed to parse %u\n", (unsigned int)packetbuf_datalen()); 
#if IEEE8023_MAC_ADDRESS_FILTER
  } else if(!linkaddr6_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER),
      &linkaddr6_node_addr) &&
      !linkaddr6_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER),
      &linkaddr6_null)) {
    /*\note The framer should have done the multicast address
     * translation from 0x33 0x33 0xXX... to the 802.3xx null
     */     
    PRINTF("ieee8023_mac: not for us\n");
#endif /* IEEE8023_MAC_ADDRESS_FILTER */                
  } else {
    int duplicate = 0;
    duplicate = mac_sequence_is_duplicate();

    if(duplicate && ETH_SUPPORT_DUPLICATE_DETECTION) {
      /* Drop the packet. */
      PRINTF("ieee8023_mac: drop duplicate link layer packet %u\n",
      packetbuf_attr(PACKETBUF_ATTR_PACKET_ID));
    } else {
      mac_sequence_register_seqno();
    }
#if DEBUG
    int i;
    uint16_t original_datalen = packetbuf_totlen();
    uint8_t *original_dataptr = packetbuf_hdrptr();
    PRINTF("PKT[%u]:",original_datalen);
    for (i=0; i<original_datalen; i++) {
      PRINTF("%02x ",original_dataptr[i]);
    }
    PRINTF("\n");
#endif

    if(!(duplicate && ETH_SUPPORT_DUPLICATE_DETECTION)) {
      link_if_in(NETSTACK_8023);
      NETSTACK_0_NETWORK.input();
      link_if_in(NETSTACK_NULL);
    } else {
      PRINTF("IEEE8023_MAC: drop duplicate\n");
    }
  }       
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
  PRINTF("IEEE8023_MAC: init\n");
}
/*---------------------------------------------------------------------------*/
static int
off(int keep_link_resources)
{
  return ETHERNET_STACK.off(keep_link_resources);
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  return ETHERNET_STACK.on();
}
/*---------------------------------------------------------------------------*/
static void
connect_event(uint8_t up)
{
  PRINTF("IEEE8023_mac: connect: %u\n", up);
  //NETSTACK_0_NETWORK.connect_event(up);
}
/*---------------------------------------------------------------------------*/
const struct mac_driver ieee8023_mac_driver = {
  (char*)"IEEE8023-mac",
  init,
  send_packet,
  packet_input,
  on,
  off,
  NULL,
  connect_event,
};
/*---------------------------------------------------------------------------*/
#endif /* WITH_ETHERNET_SUPPORT */