/*
 * platform_conf.h
 *
 * Created: 2015-10-25 00:24:37
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */


#ifndef PLATFORM_CONF_H_
#define PLATFORM_CONF_H_

#include <stdint.h>
#include "sysclk.h"
#include "gpbr.h"

typedef unsigned long clock_time_t;

#define CLOCK_CONF_SECOND                    1000

#define CCIF
#define CLIF

typedef unsigned short uip_stats_t;

/* --------------------------- RTimer Settings. ---------------------------- */
/*
 * RTimer resolution: 15MHz [RTIMER_SECOND: 15000000]
 * A tick is ~67nanoseconds
 *
 * Slow RTimer: 937.5kHz [RTIMER_SECOND_L: 937500]
 * A tick is ~1microsecond
 */
#define RTIMER_ARCH_CONF_RESOLUTION          1
#define RTIMER_ARCH_CONF_RESOLUTION_L        3

#define RTIMER_ARCH_CONF_TC0_PRIORITY        1
#define RTIMER_ARCH_CONF_TCX_PRIORITY        2

#define RTIMER_CLOCK_LT(a,b)                 ((signed short)((a)-(b)) < 0)

typedef uint64_t rtimer_clock_t;

/* -------------------------- Watchdog Settings. --------------------------- */

/** Watchdog period 5000ms */
#define WDT_PERIOD_MS					         5000
#define WATCHDOG_CONF_STATE_REG              GPBR0

/*----------------------------- UART Settings. ------------------------------*/
#define UART_CONF_IRQ_PRIORITY               4

#endif /* PLATFORM-CONF_H_ */