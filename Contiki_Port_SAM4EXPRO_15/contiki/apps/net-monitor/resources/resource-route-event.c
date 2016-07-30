#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rest-engine.h"
#include "er-coap.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define SPRINT6ADDR(addr) sprint_addr6(buf, addr)
#else
#define PRINTF(...)
#define SPRINT6ADDR(addr)
#endif
/*---------------------------------------------------------------------------*/
static void
sprint_addr6(char *buf, const uip_ipaddr_t *addr)
{
  if(addr == NULL || addr->u8 == NULL) {
    PRINTF("(NULL IP addr)");
    return;
  }
  uint16_t a;
  unsigned int i;
  int f;
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) {
        sprintf(buf, "::");
      }
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        sprintf(buf, ":");
      }
      sprintf(buf, "%x", a);
    }
  }
}
/*---------------------------------------------------------------------------*/
static uint16_t event_counter = 0;
static char routing_table[REST_MAX_CHUNK_SIZE];
static uint16_t rsp_length = 0;
/*---------------------------------------------------------------------------*/
static uint8_t
sprint_routes(char *msg)
{
  if (uip_ds6_route_num_routes() == 0) {
    return 0;
  }
  uint8_t index = 0;
  uint16_t offset = 0;
  uip_ds6_route_t *r;
  sprintf(msg, "EV:%0u\n", event_counter);
  offset = strlen(msg);

  for (r=uip_ds6_route_head(); r!=NULL; r=uip_ds6_route_next(r)) {
    index++;
    uip_ipaddr_t *route_ipaddr = &r->ipaddr;
    sprintf(msg+offset, "R%u:", index);
    offset = strlen(msg);
    sprint_addr6(msg+offset, route_ipaddr);
    offset = strlen(msg);
    sprintf(msg+offset, "\n");
    offset = strlen(msg);
  }
	return strlen(msg);
}
/*---------------------------------------------------------------------------*/
static void res_get_handler(void *request, void *response, uint8_t *buffer,
  uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);
/*---------------------------------------------------------------------------*/
/*
 * Example for an event resource.
 * Additionally takes a period parameter that defines the interval to call [name]_periodic_handler().
 * A default post_handler takes care of subscriptions and manages a list of subscribers to notify.
 */
EVENT_RESOURCE(res_route_print_event,
  "title=\"Event demo\";obs",
  res_get_handler,
  NULL,
  NULL,
  NULL,
  res_event_handler);
/*---------------------------------------------------------------------------*/
static void
res_get_handler(void *request, void *response, uint8_t *buffer,
  uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  
  memcpy(buffer, &routing_table[0], rsp_length);
  REST.set_response_payload(response, buffer, rsp_length);
}
/*---------------------------------------------------------------------------*/
/*
 * Additionally, res_event_handler must be implemented for each EVENT_RESOURCE.
 * It is called through <res_name>.trigger(), usually from the server process.
 */
static void
res_event_handler(void)
{
  /* Do the update triggered by the event here, e.g., sampling a sensor. */
  ++event_counter;
  
  /* Print routing table */
  rsp_length = sprint_routes(&routing_table[0]);
  
  /* Usually a condition is defined under with subscribers are notified,
   * e.g., event was above a threshold.
   */
  if(1) {
    /* Notify the registered observers which will trigger
     * the res_get_handler to create the response.
     */
    REST.notify_subscribers(&res_route_print_event);
  }
}
/*---------------------------------------------------------------------------*/