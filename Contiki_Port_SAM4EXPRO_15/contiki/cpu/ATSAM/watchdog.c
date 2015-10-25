/*
 * watchdog.c
 *
 * Created: 2015-10-25 12:16:46
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */
#include "contiki-conf.h"
#include "dev/watchdog.h"
#include "lib/assert.h"
#include "gpbr.h"
#include "wdt.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/** GPBR register used for storing watchdog state. */
#ifdef WATCHDOG_CONF_STATE_REG
#define WATCHDOG_STATE_REG      WATCHDOG_CONF_STATE_REG
#endif /* WATCHDOG_CONF_STATE_REG */

#define WATCHDOG_STATE_DISABLED 0
#define WATCHDOG_STATE_ENABLED  1
/*---------------------------------------------------------------------------*/
void
WDT_Handler(void)
{
  /* Watchdog interrupt */
  PRINTF("Enter watchdog interrupt.\n");

  /* Clear status bit to acknowledge interrupt by dummy read. */
  wdt_get_status(WDT);
  /* Restart the WDT counter. */
  wdt_disable(WDT);
  /* Software reset should occur. */
}
/*---------------------------------------------------------------------------*/
void
watchdog_init(void)
{
  return;
}
/*---------------------------------------------------------------------------*/
void
watchdog_periodic(void)
{
  /* This function is called periodically to restart the watchdog timer. */
  wdt_restart(WDT);
}
/*---------------------------------------------------------------------------*/
void
watchdog_start(void)
{
  uint32_t wdt_mode, wdt_timeout;

  /* Disable WDT interrupts. */
  NVIC_DisableIRQ(WDT_IRQn);

  /* Get timeout value. */
  wdt_timeout = wdt_get_timeout_value(WDT_PERIOD_MS * 1000,
    BOARD_FREQ_SLCK_XTAL);
  assert(wdt_timeout != WDT_INVALID_ARGUMENT);

  /* Configure watchdog to trigger interrupt, and  software reset. */
  wdt_mode = WDT_MR_WDFIEN |  /* Enable WDT fault interrupt. */
             WDT_MR_WDRSTEN |
             WDT_MR_WDRPROC   |  /* WDT fault resets processor only. */
             WDT_MR_WDDBGHLT  |  /* WDT stops in debug state. */
             WDT_MR_WDIDLEHLT;   /* WDT stops in idle state. */

  /* Enable watchdog timer. */
  wdt_init(WDT, wdt_mode, wdt_timeout, wdt_timeout);

  PRINTF("watchdog: enable with %lu microseconds period.\n",
    (unsigned long)wdt_get_us_timeout_period(WDT, BOARD_FREQ_SLCK_XTAL));

  /* Store watchdog state */
#ifdef WATCHDOG_STATE_REG
  gpbr_write(WATCHDOG_STATE_REG, WATCHDOG_STATE_ENABLED);
  assert(gpbr_read(WATCHDOG_STATE_REG) == WATCHDOG_STATE_ENABLED);
#endif

  /* Enable interrupt and set priority level. */
  NVIC_SetPriority(WDT_IRQn, 0);
  NVIC_EnableIRQ(WDT_IRQn);
}
/*---------------------------------------------------------------------------*/
void
watchdog_stop(void)
{
  /* Watchdog timer cannot be disabled in non-idle mode, so we disable IRQ. */
  NVIC_DisableIRQ(WDT_IRQn);
}
/*---------------------------------------------------------------------------*/