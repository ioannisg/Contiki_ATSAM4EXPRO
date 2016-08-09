/*
 * resource-route.c
 *
 * Created: 2015-01-17 23:16:06
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */
#include "contiki-net.h"
#include "ipv6/uip-ds6-route.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "er-coap.h"
#include "rest-engine.h"
#include "apps/net-monitor/net-monitor.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define SPRINT6ADDR(addr) sprint_addr6(buf, addr)
#else
#define PRINTF(...)
#define SPRINT6ADDR(addr)
#endif

#define RESOURCE_ROUTE_MAX_ROUTES          (8)
#define RESOURCE_ROUTE_MAX_ROUTE_ENTRY_LEN (32)
/*---------------------------------------------------------------------------*/
static char routing_table[REST_MAX_CHUNK_SIZE + RESOURCE_ROUTE_MAX_ROUTE_ENTRY_LEN];
static uint16_t rsp_length = 0;
/*---------------------------------------------------------------------------*/
static uint16_t
sprint_routes(char *msg)
{
  sprint_time(msg, uptime_min);   
  if (uip_ds6_route_num_routes() == 0) {
    sprintf(msg+strlen(msg), "No-routes\n");
    goto check_dft;
  }
  
  if (uip_ds6_route_num_routes() > RESOURCE_ROUTE_MAX_ROUTES) {
    sprintf(msg+strlen(msg), "Total routes:%u\n", uip_ds6_route_num_routes());
    goto check_dft;
  }
  
  uint8_t index = 0;
  uip_ds6_route_t *r;
  uip_ipaddr_t *route_ipaddr;  
  for (r=uip_ds6_route_head(); r!=NULL; r=uip_ds6_route_next(r)) {
    index++;
    route_ipaddr = &r->ipaddr;
    sprintf(msg+strlen(msg), "R%u:", index);
    sprint_addr6(msg, route_ipaddr);
    sprintf(msg+strlen(msg), "\n");
    if (strlen(msg) > REST_MAX_CHUNK_SIZE) {
      return REST_MAX_CHUNK_SIZE; 
    } 
  }
  /* Default route */
check_dft:
  sprintf(msg+strlen(msg), "DR:");
  if ((route_ipaddr = uip_ds6_defrt_choose()) != NULL) {
    sprint_addr6(msg, route_ipaddr);
  }
  /* Caller will take care of the response size */
  return strlen(msg);
}
/*---------------------------------------------------------------------------*/
static void res_get_handler(void *request, void *response, uint8_t *buffer,
  uint16_t preferred_size, int32_t *offset);
static void res_get_periodic_handler(void *request, void *response,
  uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_periodic_handler(void);
/*---------------------------------------------------------------------------*/
RESOURCE(res_routes,
  "title=\"Routing Table: ?len=0..\";rt=\"Text\"",
  res_get_handler,
  NULL,
  NULL,
  NULL);
/*---------------------------------------------------------------------------*/
static void
res_get_handler(void *request, void *response, uint8_t *buffer,
  uint16_t preferred_size, int32_t *offset)
{
  const char *len = NULL;
  memset(routing_table, 0, sizeof(routing_table));
  int length = sprint_routes(&routing_table[0]);

  /* The query string can be retrieved by rest_get_query() or parsed for its key-value pairs. */
  if(REST.get_query_variable(request, "len", &len)) {
    length = atoi(len);
    if(length < 0) {
      length = 0;
    }
    if(length > REST_MAX_CHUNK_SIZE) {
      length = REST_MAX_CHUNK_SIZE;
    }
    memcpy(buffer, &routing_table[0], length);
  } else {
    length = (length > REST_MAX_CHUNK_SIZE) ? REST_MAX_CHUNK_SIZE : length;
    memcpy(buffer, &routing_table[0], length);
  }
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN); /* text/plain is the default, hence this option could be omitted. */
  REST.set_header_etag(response, (uint8_t *)&length, 1);
  REST.set_response_payload(response, buffer, length);
}
/*---------------------------------------------------------------------------*/
PERIODIC_RESOURCE(res_routes_periodic,
  "title=\"Periodic Routing Table\";obs",
  res_get_periodic_handler,
  NULL,
  NULL,
  NULL,
  60 * CLOCK_SECOND,
  res_periodic_handler);
/*---------------------------------------------------------------------------*/
static void
res_get_periodic_handler(void *request, void *response, uint8_t *buffer,
  uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_header_max_age(response, res_routes_periodic.periodic->period / CLOCK_SECOND);
  
  memcpy(buffer, routing_table, rsp_length);
  REST.set_response_payload(response, buffer, rsp_length);
}
/*---------------------------------------------------------------------------*/
static void
res_periodic_handler(void)
{
  /* Print next route in the table */
  memset(routing_table, 0, sizeof(routing_table));
  if ((rsp_length = sprint_routes(routing_table)) > REST_MAX_CHUNK_SIZE)
    rsp_length = REST_MAX_CHUNK_SIZE;
  
  /* Usually a condition is defined under with subscribers are notified,
   * e.g., large enough delta in sensor reading.
   */
  if(1) {
    /* Notify the registered observers which will trigger,
     * the res_get_handler to create the response.
     */
    REST.notify_subscribers(&res_routes_periodic);
  }
}
/*---------------------------------------------------------------------------*/