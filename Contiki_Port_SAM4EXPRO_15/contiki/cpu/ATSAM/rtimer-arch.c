/*
 * rtimer_arch.c
 *
 * Created: 2015-10-26 20:19:50
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */
#include "rtimer-arch.h"
#include "lib/assert.h"

#include "pmc.h"
#include "tc.h"
#include "sam4e16e.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#ifdef RTIMER_ARCH_CONF_TC0_PRIORITY
#define RTIMER_ARCH_TC0_PRIORITY RTIMER_ARCH_CONF_TC0_PRIORITY
#else
#define RTIMER_ARCH_CONF_TC0_PRIORITY 1
#endif

#ifdef RTIMER_ARCH_CONF_TCX_PRIORITY
#define RTIMER_ARCH_TCX_PRIORITY RTIMER_ARCH_CONF_TCX_PRIORITY
#else
#define RTIMER_ARCH_CONF_TCX_PRIORITY 2
#endif

/* Most significant bits of the current time */
static volatile uint32_t time_msb = 0;

/*---------------------------------------------------------------------------*/
void
rtimer_arch_init(void)
{
  /* Enable peripheral clock on component: TC0, channel 0 [see sam4e16e.h] */
  assert(pmc_enable_periph_clk(ID_TC0) == 0);
  /* Enable peripheral clock on component: TC0, channel 1 [see sam4e16e.h] */
  assert(pmc_enable_periph_clk(ID_TC1) == 0);
  /* Enable peripheral clock on component: TC0, channel 2 [see sam4e16e.h] */
  assert(pmc_enable_periph_clk(ID_TC2) == 0);

  /* Configure TC0 for RTIMER_ARCH_RESOLUTION Hz and trigger on RC register compare */
  tc_init(TC0, 0, RTIMER_ARCH_PRESCALER | TC_CMR_CPCTRG);
  /* Configure TC1 for RTIMER_ARCH_RESOLUTION Hz and trigger on RC register compare */
  tc_init(TC0, 1, RTIMER_ARCH_PRESCALER | TC_CMR_CPCTRG);
  /* Configure TC2 for RTIMER_ARCH_RESOLUTION Hz and trigger on RC register compare */
  tc_init(TC0, 2, RTIMER_ARCH_PRESCALER | TC_CMR_CPCTRG);

  /* Disable all interrupts on all counters*/
  tc_disable_interrupt(TC0, 0, 0b11111111);
  tc_disable_interrupt(TC0, 1, 0b11111111);
  tc_disable_interrupt(TC0, 2, 0b11111111);

  /* Configure the main counter to trigger an interrupt on RC compare (overflow) */
  tc_write_rc(TC0, 0, 0xffffffff);
  /* Enable TC0 interrupt on RC Compare */
  tc_enable_interrupt(TC0, 0, TC_IER_CPCS);
  /* Start the universal time counter */
  tc_start(TC0, 0);

  /* NVIC */
  NVIC_SetPriority((IRQn_Type) ID_TC0, RTIMER_ARCH_TC0_PRIORITY);
  NVIC_EnableIRQ((IRQn_Type) ID_TC0);
}
/*---------------------------------------------------------------------------*/
rtimer_clock_t
rtimer_arch_now(void)
{
  uint32_t msb, lsb;
  do {
    msb = time_msb;
    lsb = tc_read_cv(TC0, 0);
  } while(msb != time_msb);

  return (((rtimer_clock_t)msb) << 32) | (rtimer_clock_t)lsb;
}
/*---------------------------------------------------------------------------*/
void
TC0_Handler(void)
{
  volatile uint32_t tc_status, TC_value;

  /* Get the current timer counter value from a 32-bit register */
  TC_value = tc_read_cv(TC0, 0);

  /* Clear status bit to acknowledge interrupt */
  tc_status = tc_get_status(TC0, 0);

  if ((tc_status & TC_SR_CPCS) == TC_SR_CPCS) {

    PRINTF("rtimer-arch: TC0:%08x [OVF]\n", TC_value);
    /* Increment the most significant part of the universal time. */
    time_msb++;
  }
  else
  {
    /* This should not happen. */
    assert(0);
  }
}
/*---------------------------------------------------------------------------*/