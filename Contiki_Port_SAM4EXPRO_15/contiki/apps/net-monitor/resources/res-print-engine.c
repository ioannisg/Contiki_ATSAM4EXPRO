/*
 * res_print_engine.c
 *
 * Created: 2015-02-07 22:19:35
 *  Author: ioannisg
 */ 

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define SPRINT6ADDR(addr) sprint_addr6(buf, addr)
#else
#define PRINTF(...)
#define SPRINT6ADDR(addr)
#endif

#include "net/ip/uip.h"
/*---------------------------------------------------------------------------*/
void
sprint_addr6(char *buf, const uip_ipaddr_t *addr)
{
	if(addr == NULL || addr->u8 == NULL) {
		PRINTF("(NULL IP addr)");
		return;
	}
	uint16_t a;
	unsigned int i;
	int f;
	uint16_t offset;
	for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
		a = (addr->u8[i] << 8) + addr->u8[i + 1];
		if(a == 0 && f >= 0) {
			if(f++ == 0) {
				offset = strlen(buf);
				sprintf(buf+offset, "::");
			}
			} else {
			if(f > 0) {
				f = -1;
				} else if(i > 0) {
				offset = strlen(buf);
				sprintf(buf+offset, ":");
			}
			offset = strlen(buf);
			sprintf(buf+offset, "%x", a);
		}
	}
}
/*---------------------------------------------------------------------------*/
void
sprint_time(char *buf, uint32_t minutes)
{
  sprintf(buf+strlen(buf), "Uptime: %u days %u hours %u minutes\n",
    minutes/(24*60), (minutes / 60) % 24, minutes % 60);
}
/*---------------------------------------------------------------------------*/