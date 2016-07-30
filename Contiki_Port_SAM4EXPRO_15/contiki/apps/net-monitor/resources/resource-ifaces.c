/*
 * resource_ifaces.c
 *
 * Created: 2015-01-17 23:54:41
 *  Author: ioannisg
 */ 
#include "contiki-net.h"
#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "apps/net-monitor/net-monitor.h"


/*---------------------------------------------------------------------------*/
static uint8_t
print_ifaces(char *msg)
{
#if UIP_CONF_MULTI_IFACES
  uint8_t ifaces = 0;
  int i;
  for (i=0; i<UIP_DS6_IFACES_NUM; i++) {
    if (uip_ds6_ifaces[i].status > UIP_DS6_IFACE_STATUS_NOT_STARTED)
      ifaces++;
  }
#else
  uint8_t ifaces = 1;
#endif
  sprint_time(msg, uptime_min);
  sprintf(msg+strlen(msg), "Total Interfaces: %u.",ifaces);
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
RESOURCE(res_ifaces,
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
  const char *eth_stats = NULL;
  size_t var_len = 0;
  
  char message[REST_MAX_CHUNK_SIZE];
  memset(message, 0, REST_MAX_CHUNK_SIZE);
  int length = print_ifaces(message);
  
  if (var_len = REST.get_query_variable(request, "eth-stats", &eth_stats)) {
    if (strncmp(eth_stats, "eth-stats", var_len)) {
      sprintf(message+length, " ETH-STATS:%u", 0);
        //(unsigned int)enc424j600_get_overflow_counter());
		length = strlen(message);
	 }
  }

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