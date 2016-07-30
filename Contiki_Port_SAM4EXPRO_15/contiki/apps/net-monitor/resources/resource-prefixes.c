/*
 * resource_prefixes.c
 *
 * Created: 2015-01-18 00:18:35
 *  Author: Ioannis Glaropoulos
 */ 
#include "contiki-net.h"
#include "rest-engine.h"
#include "apps/net-monitor/net-monitor.h"
#include <string.h>

/*---------------------------------------------------------------------------*/
static uint16_t
print_prefixes(char *msg)
{
  sprint_time(msg, uptime_min);
  int i;
  for (i=0; i<UIP_DS6_PREFIX_NB; i++) {
    uip_ds6_prefix_t *prefix = &uip_ds6_prefix_list[i];
    if (prefix->isused) {
      sprintf(msg+strlen(msg), "Addr: ");
      sprint_addr6(msg, &prefix->ipaddr);
#if UIP_MULTI_IFACES
      uint8_t if_type = prefix->iface == NULL ? 0 : prefix->iface->ll_type;
      sprintf(msg+strlen(msg), " Len: %u, IF: %u\n", prefix->length, if_type);
#else
      sprintf(msg+strlen(msg), "\n");
#endif
	 }
  }
  return strlen(msg);
}
/*---------------------------------------------------------------------------*/
static void res_get_handler(void *request, void *response, uint8_t *buffer,
  uint16_t preferred_size, int32_t *offset);
/*---------------------------------------------------------------------------*/
/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
RESOURCE(res_prefixes,
         "title=\"Hello world: ?len=0..\";rt=\"Text\"",
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
  char message[REST_MAX_CHUNK_SIZE];
  memset(message, 0, REST_MAX_CHUNK_SIZE);
  int length = print_prefixes(&message[0]);
  /* The query string can be retrieved by rest_get_query() or parsed for its key-value pairs. */
  if(REST.get_query_variable(request, "len", &len)) {
    length = atoi(len);
    if(length < 0) {
      length = 0;
    }
    if(length > REST_MAX_CHUNK_SIZE) {
      length = REST_MAX_CHUNK_SIZE;
    }
    memcpy(buffer, message, length);
  } else {
    memcpy(buffer, message, length);
  }
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_header_etag(response, (uint8_t *)&length, 1);
  REST.set_response_payload(response, buffer, length);
}
/*---------------------------------------------------------------------------*/