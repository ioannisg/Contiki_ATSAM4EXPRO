/*
 * uart0.c
 *
 * Created: 2015-10-25 20:06:56
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */
#include "platform-conf.h"
#include "conf_uart_serial.h"
#include "uart0.h"
#include "stdio_serial.h"
#include "uart.h"

/* UART0 peripheral priority. */
#ifdef UART_CONF_IRQ_PRIORITY
#define UART_IRQ_PRIORITY UART_CONF_IRQ_PRIORITY
#else
#define UART_IRQ_PRIORITY 4
#endif /* UART_CONF_IRQ_PRIORITY */

void
uart0_init(unsigned long ubr)
{
  /* Enable peripheral clock. */
  sysclk_enable_peripheral_clock(CONSOLE_UART_ID);

  const usart_serial_options_t uart_serial_options = {
    .baudrate = ubr,
    .paritytype = CONF_UART_PARITY
  };

  /* UART initialization. */
  stdio_serial_init(CONF_UART, &uart_serial_options);
}
/*---------------------------------------------------------------------------*/
uint32_t
uart0_is_tx_ready(void)
{
  return uart_is_tx_ready(CONSOLE_UART);
}
/*---------------------------------------------------------------------------*/