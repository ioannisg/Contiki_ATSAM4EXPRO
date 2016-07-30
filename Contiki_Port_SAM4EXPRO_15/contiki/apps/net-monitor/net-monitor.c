/*
 * net_monitor.c
 *
 * Created: 5/8/2014 12:36:03 AM
 *  Author: Ioannis Glaropoulos
 */ 
#include "contiki-net.h"
#include "net-monitor.h"
#include "rest-engine.h"
#include <stdlib.h>
#include <string.h>

#ifndef NET_MONITOR_PERIOD_MIN
#define NET_MONITOR_PERIOD_MIN 1
#endif

#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"

#if DEBUG
#include "resolv.h"
#endif
/*---------------------------------------------------------------------------*/
PROCESS(net_monitor_process, "Net Config Monitor");
/*---------------------------------------------------------------------------*/
#if DEBUG
static void
print_net_conf(void)
{
  int i;
  uint8_t state, issued;
#if ! DEBUG
  UNUSED(issued);
#endif
  PRINTF("NET-MONITOR: IPv6 addresses: ");
#if ! UIP_MULTI_IFACES
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    issued = uip_ds6_if.addr_list[i].isused;
    if(state == ADDR_TENTATIVE || state == ADDR_PREFERRED) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("Status: %u, issued: %u\n",state, issued);
      PRINTF("\n");
    }
  }
#else  /* UIP_MULTI_IFACES */
  for (i=0; i<UIP_DS6_IFACES_NUM; i++) {
    if (uip_ds6_ifaces[i].status <= UIP_DS6_IFACE_STATUS_NOT_STARTED) {
      continue;
    }
	 PRINTF("Interface num:%u, status:%u\n", uip_ds6_ifaces[i].ll_type,
      uip_ds6_ifaces[i].status);
    int j;
    for(j=0; j<UIP_DS6_ADDR_NB; j++) {
      state = uip_ds6_ifaces[i].netif.addr_list[j].state;
      issued = uip_ds6_ifaces[i].netif.addr_list[j].isused;
      if(state == ADDR_TENTATIVE || state == ADDR_PREFERRED) {
        PRINT6ADDR(&uip_ds6_ifaces[i].netif.addr_list[j].ipaddr);
        PRINTF(" Status: %u, issued: %u\n",state, issued);
        PRINTF("\n");
      }
    }
  }
#endif /* UIP_MULTI_IFACES */
#if UIP_CONF_ROUTER		
  PRINTF("NET-MONITOR: IPv6 prefixes: ");
  for(i = 0; i < UIP_DS6_PREFIX_NB; i++) {
    PRINT6ADDR(&uip_ds6_prefix_list[i].ipaddr);
    issued = uip_ds6_prefix_list[i].isused;
    PRINTF(" Used: %u\n",issued);
	}
	PRINTF("NET-MONITOR: IPV6 routes:\n");
	uip_ds6_route_t * r;
	for (r = uip_ds6_route_head();
		r != NULL;
		r = uip_ds6_route_next(r)) {
		PRINTF("Route: ");
		PRINT6ADDR(&r->ipaddr);	
		PRINTF(" via: ");
		PRINT6ADDR(uip_ds6_route_nexthop(r));
		PRINTF("\n");
	}			
#endif	
}
#endif
/*---------------------------------------------------------------------------*/
static struct etimer _et;
net_monitor_action_t pending_action = NETOP_NOP;
uint32_t uptime_min = 0;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(net_monitor_process, ev, data)
{
  PROCESS_BEGIN();

  UNUSED(ev);
  UNUSED(data);

  etimer_set(&_et, CLOCK_SECOND*60*NET_MONITOR_PERIOD_MIN);
  
  /* Initialize the REST engine. */
  rest_init_engine();

  /* Bind the resources to their URI-Path */
  rest_activate_resource(&res_routes, (char *)"ipv6-routes/show");
  rest_activate_resource(&res_prefixes, (char *)"ipv6-prefixes");
  rest_activate_resource(&res_ifaces, (char *)"ipv6-interfaces");
  rest_activate_resource(&res_neighbors, (char *)"ipv6-neighbors");
  rest_activate_resource(&res_routes_periodic, (char *)"ipv6-routes/periodic");
  rest_activate_resource(&res_command_directive, (char *)"command-directive");
  //rest_activate_resource(&res_route_print_event, "ipv6-routes-print");

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&_et));
    uptime_min += NET_MONITOR_PERIOD_MIN;
#if DEBUG
    print_net_conf();
#endif
    //res_route_print_event.trigger();
    switch(pending_action) {
    case NETOP_REBOOT:
      //ota_flash_reset();
      break;
    default:
      break;
    }
    etimer_set(&_et, CLOCK_SECOND*60*NET_MONITOR_PERIOD_MIN);
  }
  PROCESS_END();
}	
/*---------------------------------------------------------------------------*/