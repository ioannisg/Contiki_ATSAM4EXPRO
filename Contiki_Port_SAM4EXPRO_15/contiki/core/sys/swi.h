/*
 * swi.h
 *
 * Created: 2015-10-27 22:24:06
 *  Author: ioannisg
 */ 


#ifndef SWI_H_
#define SWI_H_

#include <stdint.h>
#include "sys/clock.h"

struct swi_evt_request;
typedef struct swi_evt_request swi_evt_request_t;

typedef void (* swi_handler_t)(void *ptr);
typedef void swi_ptr_t;
typedef uint32_t swi_evt_request_id_t;

struct swi_evt_request {
	swi_evt_request_t    *next;
	swi_evt_request_id_t id;
	clock_time_t         duration;
	clock_time_t         max_wait_time;
	swi_handler_t        handler;
	swi_ptr_t            *ptr;
};

void swi_init(void);
void swi_arch_init(void);
void swi_arch_signal(void);

#define SWI_SIGNAL() swi_arch_signal()

swi_evt_request_t * swi_evt_request_pop(void);

#endif /* SWI_H_ */