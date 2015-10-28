/*
 * swi_arch.c
 *
 * Created: 2015-10-27 21:11:28
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */

#include "contiki-conf.h"
#include "lib/assert.h"
#include "sys/swi.h"

#include "sam4e16e.h"

#ifdef PEND_SV_CONF_PRIORITY
#define PEND_SV_PRIORITY PEND_SV_CONF_PRIORITY
#else
#define PEND_SV_PRIORITY 7
#endif /* PEND_SV_CONF_PRIORITY */

/*---------------------------------------------------------------------------*/
void
swi_arch_init(void)
{
  /* Assign priority */
  NVIC_SetPriority(PendSV_IRQn, PEND_SV_PRIORITY);	
}
/*---------------------------------------------------------------------------*/
void
swi_arch_signal(void)
{
  /* Set pending flag */
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
/*---------------------------------------------------------------------------*/
void
PendSV_Handler(void)
{
  swi_evt_request_t *req;
  /* Pick the top event request from the event queue. Clear IRQ flag. */
  cpu_irq_enter_critical();
  req = swi_evt_request_pop();
  SCB->ICSR = SCB_ICSR_PENDSVCLR_Msk;
  cpu_irq_leave_critical();
  /* Start timer to mark the end of the pending time slot. */

  /* Execute the handler */  
  if ((req != NULL) && (req->handler != NULL))
  {
    (req->handler)(req->ptr);
  }
  else
  {
    assert(0);
  }
  /* Remove the handler */
}
/*---------------------------------------------------------------------------*/