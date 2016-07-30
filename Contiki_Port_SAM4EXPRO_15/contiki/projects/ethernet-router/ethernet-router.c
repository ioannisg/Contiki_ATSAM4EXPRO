/*
 * ethernet_router.c
 *
 * Created: 2016-07-29 23:40:10
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 
#include "contiki.h"
#include "ethernet-router.h"
#include "ipv6/uip-ds6.h"
#include "uip6-rio.h"
#include "resolv.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"



#define ND6_PREFIX_A_FLAG (1<<6)
#define ND6_PREFIX_L_FLAG (1<<7)
#define ND6_PREFIX_R_FLAG (1<<5)

process_event_t ethernet_router_connect_event;
/*---------------------------------------------------------------------------*/
PROCESS(ethernet_router_process, "Ethernet IPv6 router");
/*---------------------------------------------------------------------------*/
static void
ethernet_router_update_global_addr(void)
{
  if (uip_ds6_get_global(ADDR_PREFERRED) != NULL) {
    PRINTF("ethernet-router: global-addr-already-set\n");
    return;
  }
  PRINTF("ethernet-router: set-global-addr-for-interface\n");
  uip_ipaddr_t global_ipaddr;
  uip_ipaddr_t *ipaddr_ptr;
  uint8_t ethernet_prefix[16] = {ETHERNET_ROUTER_PREFIX_ETH};

  ipaddr_ptr = (uip_ipaddr_t *)&ethernet_prefix[0];
  uip_ipaddr_copy(&global_ipaddr, ipaddr_ptr);

  uip_ds6_set_addr_iid(&global_ipaddr, &uip_lladdr);
  if (uip_ds6_addr_add(&global_ipaddr, 0, ADDR_PREFERRED) == NULL) {
    PRINTF("ethernet-router: could not add global ip\n");
    return;
  }
  PRINTF("ethernet-router: new-global-ip:");
  PRINT6ADDR(&global_ipaddr);
  PRINTF("\n");
}
/*---------------------------------------------------------------------------*/
static void
ethernet_router_configure_prefix(uint8_t is_advertized, uint32_t is_used)
{
  /* Add the global prefixes of this router */
  uip_ipaddr_t global_prefix;
  uip_ipaddr_t *ipaddr_ptr = NULL;
  uint8_t ethernet_prefix[16] = {ETHERNET_ROUTER_PREFIX_ETH};

  ipaddr_ptr = (uip_ipaddr_t *)&ethernet_prefix[0];
  uip_ipaddr_copy(&global_prefix, ipaddr_ptr);
  uip_ds6_prefix_t *prefix = uip_ds6_prefix_add(&global_prefix, ETHERNET_ROUTER_PREFIX_LEN, 1, ND6_PREFIX_A_FLAG | ND6_PREFIX_L_FLAG | ND6_PREFIX_R_FLAG, UIP_ND6_INFINITE_LIFETIME, UIP_ND6_INFINITE_LIFETIME);
  prefix->isused = is_used;
  prefix->advertise = is_advertized;
#if UIP_CONF_DS6_ROUTE_INFORMATION
  uip_ds6_route_info_t *rinfo = uip_ds6_route_info_add(&global_prefix, ETHERNET_ROUTER_PREFIX_LEN, ND6_PREFIX_A_FLAG | ND6_PREFIX_L_FLAG, UIP_ND6_INFINITE_LIFETIME);
  UNUSED(rinfo);
#endif
}
/*---------------------------------------------------------------------------*/
static void
ethernet_router_configure_8023(void)
{
  ethernet_router_configure_prefix(1, 1);
  /* Add the global addresses for all the active interfaces. 
   * This must be called each time an interface is registered
   */
  ethernet_router_update_global_addr();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ethernet_router_process, ev, data)
{
  PROCESS_BEGIN();
  PRINTF("ethernet-router: starts...\n");
  ethernet_router_connect_event = process_alloc_event();
  /* Start upper layer networking. Normally TCP-IP is already running. */
  if (unlikely(!process_is_running(&tcpip_process)))
  {
    PRINTF("ethernet-router: tcpip-not-running\n");
    process_start(&tcpip_process, NULL);
  }
  /* Configure the IPv6 802.3 interface */
  ethernet_router_configure_8023();
  PROCESS_PAUSE();

  /* Start the network configuration & monitor process */
  //process_start(&net_monitor_process, NULL);
  /* Start mDNS */
  process_start(&resolv_process, NULL);
  /* Start test COAP client */
  //process_start(&coap_client_test_process, NULL);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/