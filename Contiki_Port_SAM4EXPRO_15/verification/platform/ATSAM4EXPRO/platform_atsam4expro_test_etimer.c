/*
 * platform_atsam4expro_test_etimer.c
 *
 * Created: 2015-10-25 20:50:14
 *  Author: ioannisg
 */


#include "contiki.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);

static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("Hello, world\n");

  etimer_set(&et, CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_set(&et, CLOCK_SECOND);
	 PRINTF("hello-world\n");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
