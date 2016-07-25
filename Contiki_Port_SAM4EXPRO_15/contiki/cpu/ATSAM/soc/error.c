/*
 * error.c
 *
 * Created: 2016-06-09 00:05:32
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 
#include "interrupt_sam_nvic.h"
#include "lib/assert.h"
#include <stdio.h>
/*---------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
  printf("Hard fault\n");
  while(1);
}
/*---------------------------------------------------------------------------*/
void assert_arch(void)
{
  /* Disable interrupts */
  cpu_irq_disable();
  HardFault_Handler();
}
/*---------------------------------------------------------------------------*/
