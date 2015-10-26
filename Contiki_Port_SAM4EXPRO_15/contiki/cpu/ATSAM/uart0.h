/*
 * uart0.h
 *
 * Created: 2015-10-25 20:05:10
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */


#ifndef UART0_H_
#define UART0_H_


void uart0_set_input(int (*input) (unsigned char c));

void uart0_writeb(unsigned char c);

void uart0_init(unsigned long ubr);

void uart0_enable_rx_interrupt(void);

void uart0_reset(void);

uint32_t uart0_is_tx_ready(void);

#endif /* UART0_H_ */