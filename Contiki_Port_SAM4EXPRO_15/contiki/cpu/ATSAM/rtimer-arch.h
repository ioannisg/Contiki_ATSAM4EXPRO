/*
 * rtimer_arch.h
 *
 * Created: 2015-10-25 00:28:07
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */


#ifndef RTIMER_ARCH_H_
#define RTIMER_ARCH_H_

#include "contiki-conf.h"
#include "rtimer.h"

#define RTIMER_ARCH_RES_17_NS  1
#define RTIMER_ARCH_RES_67_NS  2
#define RTIMER_ARCH_RES_267_NS 3
#define RTIMER_ARCH_RES_1_US   4

#ifdef RTIMER_ARCH_CONF_RESOLUTION
#define RTIMER_ARCH_RESOLUTION RTIMER_ARCH_CONF_RESOLUTION
#else /* RTIMER_ARCH_CONF_RESOLUTION */
#define RTIMER_ARCH_RESOLUTION RTIMER_ARCH_RES_67_NS
#endif /* RTIMER_ARCH_CONF_RESOLUTION */

#ifdef RTIMER_ARCH_CONF_RESOLUTION_L
#define RTIMER_ARCH_RESOLUTION_L RTIMER_ARCH_CONF_RESOLUTION_L
#else /* RTIMER_ARCH_CONF_RESOLUTION_L */
#define RTIMER_ARCH_RESOLUTION_L RTIMER_ARCH_RES_1_US
#endif /* RTIMER_ARCH_CONF_RESOLUTION_L */

/*
 * We use the customized pre-scaling options defined in tc.h
 * and we adjust the declarations of TC ticks per second. We
 * note that the lower the pre-scaler the higher the obtained
 * resolution, however the overhead increases as TC comparison
 * and overflow events become more frequent and the total time
 * available for universal time, stored in one 64-bit integer.
 */

#if RTIMER_ARCH_RESOLUTION == RTIMER_ARCH_RES_17_NS
#define RTIMER_ARCH_PRESCALER     0  /* Division with 2 */
#define RTIMER_ARCH_SECOND	       60000000    /* One tick: 16.66ns. CPU Clock is 120MHz. */
#endif /* RT_RESOLUTION == RES_17NS */

#if RTIMER_ARCH_RESOLUTION == RTIMER_ARCH_RES_67_NS
#define RTIMER_ARCH_PRESCALER     1 /* Division with 8 */
#define RTIMER_ARCH_SECOND        15000000    /* One tick: 66.66ns. CPU Clock is 120MHz. */
#endif /* RT_RESOLUTION == RES_67_NS */

#if RTIMER_ARCH_RESOLUTION == RTIMER_ARCH_RES_267_NS
#define RTIMER_ARCH_PRESCALER     2 /* Division with 32 */
#define RTIMER_ARCH_SECOND        3750000    /* One tick: 266.66ns. CPU Clock is 120MHz. */
#endif /* RT_RESOLUTION == RES_267_NS */

#if RTIMER_ARCH_RESOLUTION == RTIMER_ARCH_RES_1_US
#define RTIMER_ARCH_PRESCALER     3 /* Division with 128 */
#define RTIMER_ARCH_SECOND        937500    /* One tick: 1.066us. CPU Clock is 120MHz. */
#endif /* RTIMER_ARCH_RES_1_US */

#if RTIMER_ARCH_RESOLUTION_L == RTIMER_ARCH_RES_17_NS
#define RTIMER_ARCH_PRESCALER_L   0  /* Division with 2 */
#define RTIMER_ARCH_SECOND_L      60000000    /* One tick: 16.66ns. CPU Clock is 120MHz. */
#endif /* RT_RESOLUTION == RES_17NS */

#if RTIMER_ARCH_RESOLUTION_L == RTIMER_ARCH_RES_67_NS
#define RTIMER_ARCH_PRESCALER_L   1 /* Division with 8 */
#define RTIMER_ARCH_SECOND_L      15000000    /* One tick: 66.66ns. CPU Clock is 120MHz. */
#endif /* RT_RESOLUTION == RES_67_NS */

#if RTIMER_ARCH_RESOLUTION_L == RTIMER_ARCH_RES_267_NS
#define RTIMER_ARCH_PRESCALER_L   2 /* Division with 32 */
#define RTIMER_ARCH_SECOND_L      3750000    /* One tick: 266.66ns. CPU Clock is 120MHz. */
#endif /* RT_RESOLUTION == RES_267_NS */

#if RTIMER_ARCH_RESOLUTION_L == RTIMER_ARCH_RES_1_US
#define RTIMER_ARCH_PRESCALER_L   3 /* Division with 128 */
#define RTIMER_ARCH_SECOND_L      937500    /* One tick: 1.066us. CPU Clock is 120MHz. */
#endif /* RTIMER_ARCH_RES_1_US */

rtimer_clock_t rtimer_arch_now(void);

#endif /* RTIMER-ARCH_H_ */