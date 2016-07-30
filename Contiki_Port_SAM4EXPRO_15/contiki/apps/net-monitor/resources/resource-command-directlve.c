/*
 * resource_command_directlve.c
 *
 * Created: 2015-01-20 21:06:32
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */
#include "contiki.h"

#include <string.h>
#include "rest-engine.h"
#include "apps/net-monitor/net-monitor.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static void res_post_put_handler(void *request, void *response,
  uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
/*---------------------------------------------------------------------------*/
RESOURCE(res_command_directive,
  "title=\"LEDs: POST/PUT stats=eth mode=reboot|reset|nop\";rt=\"Control\"",
  NULL,
  res_post_put_handler,
  res_post_put_handler,
  NULL);
/*---------------------------------------------------------------------------*/
static void
res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *mode = NULL;
  const char *stats = NULL;
  int success = 1;
  uint32_t eth_stats;
  char msg[12];
  uint8_t length;
  
  if (len = REST.get_post_variable(request, "stats", &stats)) {
    if (strncmp(stats, "eth", len) == 0) {
      eth_stats = 0;//;enc424j600_get_overflow_counter();
      sprintf(msg, "ETH-OVF:%u", eth_stats);
      length = strlen(msg);
     }
  }
  memcpy(buffer, msg, length);
	  
  if (len = REST.get_post_variable(request, "mode", &mode)) {
    PRINTF("mode %s\n", mode);

    if (strncmp(mode, "reboot", len) == 0) {
      pending_action = NETOP_REBOOT;
    } else if (strncmp(mode, "nop", len) == 0) {
      pending_action = NETOP_NOP;
    } else if (strncmp(mode, "reset", len) == 0) {
      pending_action = NETOP_RESET_WIRELESS;	 
    } else {
      success = 0;
    }
  }
  if(!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN); /* text/plain is the default, hence this option could be omitted. */
  REST.set_header_etag(response, (uint8_t *)&length, 1);
  REST.set_response_payload(response, buffer, length);
}
/*---------------------------------------------------------------------------*/