/*
 * contiki_main.c
 *
 * Created: 2015-10-25 19:49:58
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */
#include "contiki.h"
#include "dev\watchdog.h"
#include "uart0.h"
#include "conf_uart_serial.h"


#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


/*---------------------------------------------------------------------------*/
static void
hw_init(void)
{
  sysclk_init();

  irq_initialize_vectors();
  cpu_irq_enable();

  board_init();
}
/*---------------------------------------------------------------------------*/
int
main(void)
{
  /* Initialize the SAM system. */
  hw_init();

  /* Serial line [UART] initialization */
  uart0_init(CONF_UART_BAUDRATE);

#if WITH_SERIAL_LINE_INPUT
  uart0_set_input(serial_line_input_byte);
  serial_line_init();
#endif

  while(!uart0_is_tx_ready());

  /* PRINT Contiki Entry String */
  PRINTF("Starting ");
  PRINTF(CONTIKI_VERSION_STRING);

  /* Configure sys-tick for 1 ms */
  clock_init();

  /* Initialize Contiki and our processes */
  process_init();

  /* etimer_process should be initialized before ctimer */
  process_start(&etimer_process, NULL);

  /* Initialize the ctimer process */
  ctimer_init();

  /* Initialize the rtimer library */
  rtimer_init();

  /* Initialize watch-dog timer */
  watchdog_start();

  /* Start all auto-start processes. */
  autostart_start(autostart_processes);

  while(1) {
    /* Reset watchdog. */
    watchdog_periodic();
    /* Contiki Polling System */
    process_run();
  }
}
/*---------------------------------------------------------------------------*/