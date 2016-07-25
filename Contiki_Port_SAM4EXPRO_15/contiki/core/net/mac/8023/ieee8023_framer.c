/*
 * ieee8023_framer.c
 *
 * Created: 5/2/2014 5:36:38 PM
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 *
 * An IEEE 802.3 framer including minimal filtering functionality.
 */ 

/*
 * Documentation:
 * parse():
 * The function creates 6-byte long link addresses (linkaddr0_t types) and
 * copies the relevant contents of the incoming frame header, namely the 6
 * byte long Ethernet headers. If the incoming receiver address has the IPv6
 * multicast type, the receiver address is converted to the 6-byte ZEROS one.
 * The actual sizes of the packet buffer fields MAY be 8 bytes long, so it is
 * the responsibility of upper layer NETSTACK modules to resolve this issue.
 
 * create():
 * The function reads the contents of the packet buffer fields and creates an
 * Ethernet header, with 6-byte long MAC headers. After the packet is created
 * the Receiver address is converted to the standard ZEROS address, if it was
 * a multicast one. So the upper NETSTACK modules can call the "sent" callback
 * with a RECEIVER address already prepared. Notice, however, that this is a 6
 * byte long address, so it is the responsibility of the upper modules to pad,
 * if required.
 */
#include "contiki-net.h"
#include "net/mac/framer.h"
#include "mini_ip.h"
#include "dev/ethernetstack.h"

#include "net/linkaddr6.h"
#include "net/packetbuf.h"
#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINTADDR(addr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5])
#else
#define PRINTF(...)
#define PRINTADDR(addr)
#endif


#if WITH_ETHERNET_SUPPORT

/*---------------------------------------------------------------------------*/
static int
hdr_length(void)
{
  return sizeof(ethernet_header_t);
}
/*---------------------------------------------------------------------------*/
static int
parse(void)
{
  if (packetbuf_datalen() < sizeof(ethernet_header_t)) {
    PRINTF("IEEE8023_FRAMER: too-short pkt [%u]\n",packetbuf_datalen());
    return FRAMER_FAILED;
  }
  ethernet_header_t *hdr = packetbuf_hdrptr();

  /* Register addresses */
  linkaddr6_t srcaddr, destaddr;

  linkaddr6_copy(&srcaddr, (linkaddr6_t*)(&hdr->et_src[0]));
  linkaddr6_copy(&destaddr, (linkaddr6_t*)(&hdr->et_dest[0]));

  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, (linkaddr_t*)&srcaddr);
  
  /* Filter out non-uni-cast/multi-cast packets */
  if( (destaddr.u8[0] != 0x33 || destaddr.u8[0] != 0x33) &&
    !linkaddr6_cmp(&destaddr, &linkaddr6_node_addr)) {
    PRINTF("IEEE8023_FRAMER: drop-pkt-not-for-us:");
    PRINTADDR(&destaddr);
    PRINTF("\n");   
    return FRAMER_FAILED;
  } else {
    if (destaddr.u8[0] == 0x33 && destaddr.u8[0] == 0x33) {
      /* Modify destination address to the broadcast ETH address */
      packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, (linkaddr_t*)&linkaddr6_null);
    } else {
      packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, (linkaddr_t*)&linkaddr6_node_addr);
    }
  }

  /* We only process IPv6 packets */
  uint16_t us_pkt_format;
  us_pkt_format = SWAP16(hdr->et_protlen);

  switch (us_pkt_format) {
    /* ARP Packet format */
  case ETH_PROT_ARP:
    PRINTF("IEEE8023_FRAMER: got ARP\n");
    return FRAMER_FAILED;
  case ETH_PROT_IP:
#if NETSTACK_CONF_WITH_IPV6
    PRINTF("IEEE8023_FRAMER: got IPv4-not-supported\n");
    return FRAMER_FAILED;
#endif
  case ETH_PROT_IPV6:
    PRINTF("IEEE8023_FRAMER: got IP\n");
    break;  
  default:
	PRINTF("IEEE8023_FRAMER: unrecognized protocol header\n");
   return FRAMER_FAILED;
  }
  PRINTF("IEEE8023-IN: ");
  PRINTADDR(packetbuf_addr(PACKETBUF_ADDR_SENDER));
  PRINTADDR(packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
  PRINTF("[%u]\n", packetbuf_datalen());

  /* Header parsed & removed */
  if (packetbuf_hdrreduce(sizeof(ethernet_header_t)))
    return sizeof(ethernet_header_t); 

  return FRAMER_FAILED;   
}
/*---------------------------------------------------------------------------*/
static int
create(void)
{
	uint8_t hdr_len = sizeof(ethernet_header_t);
	int result;
	
	if(packetbuf_hdralloc(hdr_len)) {
		ethernet_header_t *hdr = packetbuf_hdrptr();
      memset(hdr, 0, sizeof(ethernet_header_t));
      /* Fill-in the 802.3 header  */
      linkaddr6_copy((linkaddr6_t*)(hdr->et_src),
			(linkaddr6_t*)packetbuf_addr(PACKETBUF_ADDR_SENDER));
      linkaddr6_copy((linkaddr6_t*)(hdr->et_dest),
			(linkaddr6_t*)packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
		hdr->et_protlen = SWAP16(packetbuf_attr(PACKETBUF_ATTR_ETH_PROTO));
#if DEBUG
		int i;
      PRINTF("IEEE8023_FRAMER: (%u) ",packetbuf_totlen());
      uint8_t *frame_data = packetbuf_hdrptr();
      for (i=0; i<packetbuf_totlen(); i++)
			PRINTF("%02x ",frame_data[i]);
      PRINTF("\n");
#endif
		result = hdr_len;
	} else {
		PRINTF("802.3-OUT: too large header: %u\n", hdr_len);
      result = FRAMER_FAILED;
	}
	/* For broadcast IPv6 Ethernet packets, we need to REVERT to the standard
	 * null interface (linkaddr0_null) address.
	 */
	if(((linkaddr6_t*)packetbuf_addr(PACKETBUF_ADDR_RECEIVER))->u8[0] == 0x33 &&
		((linkaddr6_t*)packetbuf_addr(PACKETBUF_ADDR_RECEIVER))->u8[1] == 0x33) {
			PRINTF("IEEE8023_FRAMER: Reverting to standard NULL linkaddr\n");
			packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, 
				(linkaddr_t*)&linkaddr6_null);
	}
	return result;
}
/*---------------------------------------------------------------------------*/
const struct framer ieee8023_framer = {
    hdr_length,
    create,
    parse
};
#endif /* WITH_ETHERNET_SUPPORT */