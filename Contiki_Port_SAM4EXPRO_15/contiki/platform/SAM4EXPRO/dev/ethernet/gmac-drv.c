/*
 * gmac_drv.c
 *
 * Created: 2015-11-15 23:50:24
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */
#include "assert.h"
#include "net\linkaddr6.h"
#include "net\netstack_x.h"
#include "packetbuf.h"
#include "dev\ethernetstack.h"
#include "dev\watchdog.h"
#include "contiki-net.h"

#include "conf_eth.h"
#include "gmac_raw.h"
#include "ethernet_phy.h"
#include "gmac-drv.h"

#include <string.h>

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


PROCESS(gmac_driver_process, "GMAC_driver_process");
/*---------------------------------------------------------------------------*/

/** The GMAC driver instance */
static gmac_device_t gs_gmac_dev;

/** The driver state & statistics context */
static gmac_drv_t gmac_drv_device = {
  .state = GMAC_DRV_STATE_NOT_INITIALIZED,
};

static uint8_t gs_uc_mac_address[] = {
  ETHERNET_CONF_ETHADDR0,
  ETHERNET_CONF_ETHADDR1,
  ETHERNET_CONF_ETHADDR2,
  ETHERNET_CONF_ETHADDR3,
  ETHERNET_CONF_ETHADDR4,
  ETHERNET_CONF_ETHADDR5
};
/*---------------------------------------------------------------------------*/
static void
gmac_dev_rx_callback(uint32_t status)
{
  assert((gmac_drv_device.state & ((GMAC_DRV_STATE_NOT_INITIALIZED) | (GMAC_DRV_STATE_PENDING_RESET))) == 0);

  /* Invoke poll handler if packet is received successfully */
  if (GMAC_RSR_REC == status)
  {
    gmac_drv_device.stats.rx++;
  }
  else
  {
    PRINTF("gmac-drv: err:%lu\n", status);
    if (GMAC_RSR_BNA == (GMAC_RSR_BNA & status))
    {
      gmac_drv_device.stats.rx_err_no_buf++;
    }
    else if (GMAC_RSR_RXOVR == (GMAC_RSR_RXOVR & status))
    {
      gmac_drv_device.stats.rx_err_overrun++;
      /* TODO Consider resetting the device */
    }
    else
    {
      assert(0);
    }
  }
  /* Set state to pending */
  gmac_drv_device.state |= GMAC_DRV_STATE_PENDING_RX;
  process_poll(&gmac_driver_process);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief GMAC interrupt handler.
 */
void GMAC_Handler(void)
{
  gmac_handler(&gs_gmac_dev);
}
/*---------------------------------------------------------------------------*/
static void
gmac_driver_init(void)
{
  assert(gmac_drv_device.state == GMAC_DRV_STATE_NOT_INITIALIZED);

  gmac_options_t m_gmac_option;

  /* Enable broadcast reception */
  m_gmac_option.uc_no_boardcast = 0;
  /* Enable promiscuous mode */
  m_gmac_option.uc_copy_all_frame = 1;
  /* Set default MAC address */
  memcpy(m_gmac_option.uc_mac_addr, gs_uc_mac_address, sizeof(gs_uc_mac_address));

  /* Enable GMAC clock */
  pmc_enable_periph_clk(ID_GMAC);
  /* Populate g_mac low level device */
  gs_gmac_dev.p_hw = GMAC;

  /* Init GMAC driver structure */
  gmac_dev_init(GMAC, &gs_gmac_dev, &m_gmac_option);

  /* Enable Interrupt */
  NVIC_EnableIRQ(GMAC_IRQn);

  /* Init MAC PHY driver */
  if (ethernet_phy_init(GMAC, BOARD_GMAC_PHY_ADDR, sysclk_get_cpu_hz())
    != GMAC_OK) {
    PRINTF("gmac-drv: PHY Initialize error!\r");
    assert(0);
  }

  /* Auto Negotiate, work in RMII mode */
  if (ethernet_phy_auto_negotiate(GMAC, BOARD_GMAC_PHY_ADDR) != GMAC_OK) {
    PRINTF("gmac-drv: Auto Negotiate error!\r");
    assert(0);
  }

  /* Establish Ethernet link */
  while (ethernet_phy_set_link(GMAC, BOARD_GMAC_PHY_ADDR, 1) != GMAC_OK) {
    PRINTF("gmac-drv: Set link error!\r");
    assert(0);
  }

  /* Disable jumbo frames */
  gmac_enable_jumbo_frames(GMAC, 0);

  /* Enable big frames */
  gmac_enable_big_frame(GMAC, 0);

  /* Start driver process */
  process_start(&gmac_driver_process, NULL);
}
/*---------------------------------------------------------------------------*/
static void
gmac_driver_reset(void)
{
  /* TODO Reset the Ethernet device */
}
/*---------------------------------------------------------------------------*/
static void
gmac_driver_set_mac_address(linkaddr6_t *p_mac_addr)
{
  gmac_set_address(GMAC, 0, &p_mac_addr->u8[0]);
  linkaddr6_copy((linkaddr6_t *)&gs_uc_mac_address[0], p_mac_addr);
}
/*---------------------------------------------------------------------------*/
static linkaddr6_t*
gmac_driver_get_mac_address(void)
{
  return (linkaddr6_t *)&gs_uc_mac_address[0];
}
/*---------------------------------------------------------------------------*/
gmac_driver_is_initialized(void)
{
  return !(gmac_drv_device.state & GMAC_DRV_STATE_NOT_INITIALIZED);
}
/*---------------------------------------------------------------------------*/
static void
gmac_driver_pollhandler(void)
{
  uint32_t rx_status;
  uint32_t rx_len = 0;

  assert((gmac_drv_device.state & GMAC_DRV_STATE_PENDING_RX) != 0);

  /* All packets will be processed before return, so we clear the state indicator now. FIXME place inside critical region. */
  gmac_drv_device.state &= ~(GMAC_DRV_STATE_PENDING_RX);

  /* RX Event occurred */
  while(1)
  {
    watchdog_periodic();
    /* Clear packet buffer contents */
	 packetbuf_clear();
    /* packet buffer mush have sufficient size to accommodate the received frame */
    rx_status = gmac_dev_read(&gs_gmac_dev, (uint8_t *)packetbuf_dataptr(), ETHERNET_MAX_FRAME_LEN, &rx_len);
    if (rx_status != GMAC_OK)
    {
      if (rx_status != GMAC_RX_NO_DATA)
      {
        PRINTF("gmac-drv. read-error:%lu\n", rx_status);
        /* TODO consider resetting the device */
      }
		else
      {
        /* All packets are processed */
      }
      break;
    }
    /* Process packet */
    PRINTF("gmac-drv: process-RX[%lu]\n", rx_len);
    packetbuf_set_datalen(rx_len);
    /* Send the packet to the network stack. */
    //NETSTACK_0_MAC.input();
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gmac_driver_process, ev, data)
{
  UNUSED(data);

  /* Register poll-handler */
  PROCESS_POLLHANDLER(gmac_driver_pollhandler());

  PROCESS_BEGIN();

  PRINTF("GMAC_driver process\n");
  /* Allow for other initialization to take place */
  PROCESS_PAUSE();
 
  /* Set link layer address for Contiki NETSTACK */
  linkaddr6_set_node_addr(gmac_driver_get_mac_address());
  
  /* Flush Ethernet device and set RX callback function. */
  while(1)
  {
    uint32_t bytes_received;
    uint32_t recv_array[ETHERNET_MAX_FRAME_LEN];
    uint32_t status = gmac_dev_read(&gs_gmac_dev, (uint8_t *)&recv_array[0], ETHERNET_MAX_FRAME_LEN, &bytes_received);
    if (status != GMAC_OK)
    {
      break;
    }
  }
  gmac_dev_set_rx_callback(&gs_gmac_dev, gmac_dev_rx_callback);

  /* Enter IDLE state */
  gmac_drv_device.state = GMAC_DRV_STATE_IDLE;

  /* Notify Contiki network stack that Ethernet interface is up. */
  //NETSTACK_0_MAC.connect_event(1);

  /* Driver's loop */
  while(1)
  {
    PROCESS_WAIT_EVENT();
    if (PROCESS_EVENT_EXIT == ev)
    {
      /* Process is asked to terminate TODO turn off device */
      break;
    }
  }
  /* Set state to non-initialized */
  assert(gmac_drv_device.state != GMAC_DRV_STATE_NOT_INITIALIZED);
  gmac_drv_device.state = GMAC_DRV_STATE_NOT_INITIALIZED;
  
  PRINTF("GMAC_driver: exiting\n");
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
const struct eth_driver gmac_driver = {
  "GMAC",
  gmac_driver_init,
  gmac_driver_reset,
  gmac_driver_set_mac_address,
  gmac_driver_get_mac_address,
  NULL,
  gmac_driver_is_initialized,
  NULL,
  NULL,
  NULL,
};
/*---------------------------------------------------------------------------*/
