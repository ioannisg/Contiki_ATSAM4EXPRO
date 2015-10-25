/*
 * clock.c
 *
 * Created: 2015-10-25 00:16:36
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */
#include "contiki-conf.h"
#include "sys/etimer.h"
#include "lib/assert.h"
#include "dev/watchdog.h"

#include "delay.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* Global variable counting the system ticks since boot time */
static volatile clock_time_t count;

/* Global variable counting seconds since boot time */
static volatile unsigned long current_seconds;

/* Seconds countdown */
static unsigned short second_countdown;
/*---------------------------------------------------------------------------*/
/* SysTick Interrupt Handler */
void
SysTick_Handler(void)
{
  count++;
  second_countdown--;

  if (second_countdown == 0)
  {
    second_countdown = CLOCK_SECOND;
    current_seconds++;
  }

  if (etimer_pending() && (etimer_next_expiration_time() == count))
  {
    etimer_request_poll();
  }
}
/*---------------------------------------------------------------------------*/
void
clock_init(void)
{
  cpu_irq_enter_critical();

  count = 0;
  current_seconds = 0;
  second_countdown = CLOCK_SECOND;
  /* Configure sys-tick for 1 ms */
  PRINTF("clock: Configure system tick to get 1ms tick period.\n");
  PRINTF("clock: The output of the sysclk_get_cpu_hz() is %u Hz.\n",
    (unsigned int)sysclk_get_cpu_hz());

  if (SysTick_Config(sysclk_get_cpu_hz() / CLOCK_SECOND)) {
    assert(0);
  }
  cpu_irq_leave_critical();
}
/*---------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{
  clock_time_t t1, t2;
  do {
    t1 = count;
    t2 = count;
	} while(t1 != t2);
  return t1;
}
/*---------------------------------------------------------------------------*/
unsigned long
clock_seconds(void)
{
  unsigned long t1, t2;
  do {
    t1 = current_seconds;
    t2 = current_seconds;
  } while(t1 != t2);
  return t1;
}
/*---------------------------------------------------------------------------*/
void
clock_set_seconds(unsigned long sec)
{
  current_seconds = sec;
}
/*---------------------------------------------------------------------------*/
void
clock_wait(clock_time_t t)
{
  clock_time_t start;

  start = clock_time();
  while((clock_time() - start) < t) {
    watchdog_periodic();
  }
}
/*---------------------------------------------------------------------------*/
void
clock_delay_usec(uint16_t dt)
{
  watchdog_periodic();
  delay_us(dt);
}
/*---------------------------------------------------------------------------*/