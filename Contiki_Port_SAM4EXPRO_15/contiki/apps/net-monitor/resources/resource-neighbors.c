/*
 * resource_neighbors.c
 *
 * Created: 2015-01-18 00:47:53
 *  Author: Ioannis Glaropoulos
 */ 

#include "contiki-net.h"
#include <stdlib.h>
#include <string.h>
#include "apps/net-monitor/net-monitor.h"
#include "rest-engine.h"
#include "ipv6/uip-ds6-nbr.h"

/*---------------------------------------------------------------------------*/
static uint16_t
print_neighbors(char *msg)
{
  sprint_time(msg, uptime_min);
  sprintf(msg+strlen(msg),"N%u\n", uip_ds6_nbr_num());
  uip_ds6_nbr_t *nbr = nbr_table_head(ds6_neighbors);
  while (nbr != NULL) {
    if (strlen(msg) > REST_MAX_CHUNK_SIZE) {
      return strlen(msg);
    }
    sprint_addr6(msg, &nbr->ipaddr);
#if UIP_MULTI_IFACES
    sprintf(msg+strlen(msg), ";%u", nbr->iface->ll_type);
#endif
    sprintf(msg+strlen(msg), "\n");
    nbr = nbr_table_next(ds6_neighbors, nbr);
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
RESOURCE(res_neighbors,
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
  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. 
   * For more, see the chunk resource.
   */
  //char const *const message = "Hello World! ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxy";
  //int length = 12; /*           |<-------->| */

  char message[2*REST_MAX_CHUNK_SIZE];
  memset(message, 0, REST_MAX_CHUNK_SIZE);
  int length = print_neighbors(message);
	
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
  } REST.set_header_content_type(response, REST.type.TEXT_PLAIN); /* text/plain is the default, hence this option could be omitted. */
  REST.set_header_etag(response, (uint8_t *)&length, 1);
  REST.set_response_payload(response, buffer, length);
}
/*---------------------------------------------------------------------------*/