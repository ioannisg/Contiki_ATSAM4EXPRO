/*
 * swi.c
 *
 * Created: 2015-10-28 23:47:09
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */

#include "swi.h"
#include "list.h"
#include "memb.h"

#include "contiki-conf.h"

#ifdef SWI_CONF_MAX_EVT_REQUESTS
#define SWI_MAX_EVT_REQUESTS SWI_CONF_MAX_EXT_REQUESTS
#else
#define SWI_MAX_EVT_REQUESTS 4
#endif /* WI_MAX_EVT_REQUESTS */

/* The memory and list for holding the pending requests */
MEMB(swi_evt_mem, swi_evt_request_id_t, SWI_MAX_EVT_REQUESTS);
LIST(swi_evt_list);

/*---------------------------------------------------------------------------*/
swi_evt_request_t *
swi_evt_request_pop(void)
{
  return (swi_evt_request_t *)list_pop(swi_evt_list); 
}
/*---------------------------------------------------------------------------*/