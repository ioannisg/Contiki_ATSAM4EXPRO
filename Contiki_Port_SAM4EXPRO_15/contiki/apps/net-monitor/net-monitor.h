/*
 * net_monitor.h
 *
 * Created: 5/8/2014 12:34:46 AM
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 


#ifndef NET_MONITOR_H_
#define NET_MONITOR_H_


#include "contiki-net.h"
#include "rest-engine.h"

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern resource_t res_route_print_event, 
  res_routes_periodic, 
  res_routes, 
  res_prefixes, 
  res_ifaces, 
  res_neighbors,
  res_command_directive;
typedef enum {
  NETOP_NOP,
  NETOP_REBOOT,
  NETOP_RESET_ETH,
  NETOP_RESET_WIRELESS,
} net_monitor_action_t;

extern net_monitor_action_t pending_action;
extern uint32_t uptime_min;
 
PROCESS_NAME(net_monitor_process);

void sprint_addr6(char *buf, const uip_ipaddr_t *addr);
void sprint_time(char *buf, uint32_t minutes);
#endif /* NET-MONITOR_H_ */