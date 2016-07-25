/*
 * platform_atsam4xpro_test_rtimer.c
 *
 * Created: 2015-10-26 22:32:06
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
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
PROCESS(atsam4expro_test_rtimer_process, "Test RTIMER process");
AUTOSTART_PROCESSES(&atsam4expro_test_rtimer_process);

static struct etimer et;
static rtimer_clock_t saved_time;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(atsam4expro_test_rtimer_process, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("Hello, world\n");

  etimer_set(&et, 0.5*CLOCK_SECOND);
  saved_time = RTIMER_NOW();

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	 rtimer_clock_t current_time = RTIMER_NOW();
    etimer_set(&et, 0.5*CLOCK_SECOND);
    PRINTF("RTIMER: time difference: %llu\n", current_time-saved_time);
	 saved_time = current_time;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
