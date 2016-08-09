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
#include "contiki-lib.h"

#include "conf_eth.h"
#include "gmac_raw.h"
#include "ethernet_phy.h"
#include "gmac-drv.h"

#include <string.h>

#if WITH_ETHERNET_SUPPORT

#define DEBUG 0
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

/* Declare the TX pending list of frames (corresponding to RX addresses) */
LIST(gmac_drv_tx_pending_list);
/* Declare the static memory for the TX pending meta-data queue */
MEMB(gmac_drv_tx_pending_mem, gmac_drv_tx_pending_element_t, GMAC_DRV_TX_PENDING_MAX_QUEUE_LEN);

/*---------------------------------------------------------------------------*/
static void
gmac_dev_tx_callback(uint32_t status)
{
  assert((gmac_drv_device.state & ((GMAC_DRV_STATE_NOT_INITIALIZED) | (GMAC_DRV_STATE_PENDING_RESET))) == 0);

  if (GMAC_TSR_TXCOMP == status)
  {
    gmac_drv_device.stats.tx++;
  }
  else
  {
    PRINTF("gmac-drv: tx-comp-err:%lu\n", status);
    gmac_drv_device.stats.tx_err++;
    if (GMAC_TSR_RLE == status)
    {
      /* Retry-limit exceeded. Consider resetting the device */
      PRINTF("gmac-drv: retry-limit-exceeded\n");
    }
    else if ((GMAC_TSR_UND | GMAC_TSR_COL) & status)
    {
      /* Collision or Under-run */
    }
    else
    {
      assert(0);
    }
  }
  /* Set the next ACKED packet status. This callback is invoked for EACH
   * packet, whose transmission is completed. So it is safe to go through
   * the pending packets in the list and to acknowledge the first in line.
   */
  gmac_drv_tx_pending_element_t *p_pending_pkt = list_head(gmac_drv_tx_pending_list);
  assert(p_pending_pkt != NULL);
  uint8_t not_found = 1;
  while(p_pending_pkt != NULL) {
    if (!p_pending_pkt->status.is_acked)
    {
      p_pending_pkt->status.is_acked = 1;
      p_pending_pkt->status.status = status;
      not_found = 0;
      break;
    }
    p_pending_pkt = p_pending_pkt->next;
  }
  assert(not_found == 0);
  /* Set state to pending */
  gmac_drv_device.state |= GMAC_DRV_STATE_PENDING_TX;
  process_poll(&gmac_driver_process);
}
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
    PRINTF("gmac-drv: rx-err:%lu\n", status);
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
gmac_driver_set_mac_address(linkaddr6_t *p_mac_addr)
{
  assert(p_mac_addr != NULL);
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
static void
gmac_driver_init(void)
{
  assert((gmac_drv_device.state == GMAC_DRV_STATE_NOT_INITIALIZED) &&
    (process_is_running(&gmac_driver_process) == 0));

  /* Initialize driver TX pending list */
  list_init(gmac_drv_tx_pending_list);

  /* Initialize driver TX pending queue for packet meta-data */
  memb_init(&gmac_drv_tx_pending_mem);

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
  INTERRUPT_ENABLE(GMAC_IRQn);

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

  /* Set link layer address for Contiki NETSTACK */
  linkaddr6_set_node_addr(gmac_driver_get_mac_address());

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
static uint8_t
gmac_driver_is_initialized(void)
{
  return !(gmac_drv_device.state & GMAC_DRV_STATE_NOT_INITIALIZED);
}
/*---------------------------------------------------------------------------*/
static void
gmac_driver_tx(mac_callback_t sent, void *ptr)
{
  int mac_status = MAC_TX_OK;
  gmac_drv_tx_pending_element_t *p_tx_pkt;
  PRINTF("gmac_drv: TX[%u]\n", packetbuf_totlen());

  /* IEEE8023_mac shall currently not maintain a transmit queue. */
  assert(ptr == NULL);

  /* Store frame meta-data [receiver MAC address] */
  p_tx_pkt = (gmac_drv_tx_pending_element_t *)memb_alloc(&gmac_drv_tx_pending_mem);
  if (p_tx_pkt == NULL) {
    PRINTF("gmac-drv: outgoing TX queue limit reached; dropping frame.\n");
    mac_status = MAC_TX_ERR_FATAL;
    goto err_exit;
  }
  p_tx_pkt->next = NULL;
  p_tx_pkt->status.is_acked = 0;
  p_tx_pkt->cb = sent;
  linkaddr6_copy(&p_tx_pkt->addr, (const linkaddr6_t *)packetbuf_addr(PACKETBUF_ADDR_RECEIVER));

  /* Enqueue the frame for transmission. Submit the callback function. */
  switch(gmac_dev_write(&gs_gmac_dev, packetbuf_hdrptr(), packetbuf_totlen(), gmac_dev_tx_callback))
  {
    case GMAC_OK:
      /* All OK. */
      /* Add pending TX frame to the end of the list */
      list_add(gmac_drv_tx_pending_list, p_tx_pkt);
      break;
    case GMAC_TX_BUSY:
      mac_status = MAC_TX_ERR;
      break;
    case GMAC_TIMEOUT:
    case GMAC_PARAM:
      mac_status = MAC_TX_ERR_FATAL;
      break;
    default:
      assert(0);
      break;
  }
  if (mac_status != MAC_TX_OK)
  {
    /* Remove from queue */
    memb_free(&gmac_drv_tx_pending_mem, p_tx_pkt);
    /* Notify NETSTACK */
err_exit:
    PRINTF("gmac-drv: tx-err:%u",mac_status);
    mac_call_sent_callback(sent, NULL, mac_status, 1);
  }
}
/*---------------------------------------------------------------------------*/
static uint8_t
gmac_driver_exit(uint8_t keep_resources)
{
  UNUSED(keep_resources);

  assert(!(GMAC_DRV_STATE_NOT_INITIALIZED & gmac_drv_device.state));

  assert(PROCESS_ERR_OK ==
    process_post(&gmac_driver_process, PROCESS_EVENT_EXIT, NULL));

  return 1;
}
/*---------------------------------------------------------------------------*/
static void
gmac_driver_pollhandler(void)
{
  uint32_t rx_status;
  uint32_t rx_len = 0;

  assert((gmac_drv_device.state & (GMAC_DRV_STATE_PENDING_RX | GMAC_DRV_STATE_PENDING_TX)) != 0);

  if (gmac_drv_device.state & GMAC_DRV_STATE_PENDING_RX)
  {
    /* RX Event occurred */
    /* All packets (including those that may arrive while the poll-handler is executing),
     * will be processed before return, so it is safe to clear the state indicator now.
     */
    gmac_drv_device.state &= ~(GMAC_DRV_STATE_PENDING_RX);
    INTERRUPT_DISABLE(GMAC_IRQn);
    if ((gmac_drv_device.state & (GMAC_DRV_STATE_PENDING_RX | GMAC_DRV_STATE_PENDING_TX)) == 0)
    {
      /* Remove process poll if registered */
      gmac_driver_process.needspoll = 0;
    }
    INTERRUPT_ENABLE(GMAC_IRQn);

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
      NETSTACK_0_MAC.input();
    }
  }
  if (gmac_drv_device.state & GMAC_DRV_STATE_PENDING_TX)
  {
    /* TX complete event occurred */
    /* This loop will invoke the NETSTACK_MAC callback for all the packets
     * whose transmission has completed. So it is safe to clear the pending
     * flag and the poll signal if no other polling reason is waiting.
     */
    gmac_drv_device.state &= ~(GMAC_DRV_STATE_PENDING_TX);
    INTERRUPT_DISABLE(GMAC_IRQn);
    if ((gmac_drv_device.state & (GMAC_DRV_STATE_PENDING_RX | GMAC_DRV_STATE_PENDING_TX)) == 0)
    {
      /* Remove process poll if registered */
      gmac_driver_process.needspoll = 0;
    }
    INTERRUPT_ENABLE(GMAC_IRQn);
    /* Run over the list elements. If status is present, remove
     * element from list and invoke callback.
     */
    gmac_drv_tx_pending_element_t *p_pending_pkt = list_head(gmac_drv_tx_pending_list);
    assert(p_pending_pkt != NULL);
    uint8_t not_found = 1;
    while(p_pending_pkt != NULL) {
      if (!p_pending_pkt->status.is_acked)
      {
        break;
	   }
      not_found = 0;
      /* Invoke callback */
      assert(p_pending_pkt->cb != NULL);
      int tx_status = MAC_TX_OK;
      switch(p_pending_pkt->status.status)
      {
        case GMAC_TSR_TXCOMP:
          tx_status = MAC_TX_OK;
          PRINTF("gmac-drv: TX-comp\n");
          break;
        case GMAC_TSR_RLE:
          tx_status = MAC_TX_ERR_FATAL;
          break;
        case GMAC_TSR_COL:
        case GMAC_TSR_UND:
          tx_status = MAC_TX_ERR;
          break;
        default:
          assert(0);
          break;
      }
      mac_call_sent_callback(p_pending_pkt->cb, NULL, tx_status, 1);
      /* Free */
		memb_free(&gmac_drv_tx_pending_mem, p_pending_pkt);
		list_pop(gmac_drv_tx_pending_list);
      p_pending_pkt = p_pending_pkt->next;
    }
    assert(not_found == 0);
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gmac_driver_process, ev, data)
{
  UNUSED(data);

  /* Register poll-handler */
  PROCESS_POLLHANDLER(gmac_driver_pollhandler());

  PROCESS_BEGIN();

  PRINTF("gmac-drv: starting\n");
  /* Allow for other initialization to take place */
  PROCESS_PAUSE();
  
  /* Flush Ethernet device */
  while(1)
  {
    uint32_t bytes_received;
    uint32_t status = gmac_dev_read(&gs_gmac_dev, (uint8_t *)packetbuf_dataptr(), ETHERNET_MAX_FRAME_LEN, &bytes_received);
    if (status != GMAC_OK)
    {
      break;
    }
  }
  /*Set RX callback function for receiving frames */
  gmac_dev_set_rx_callback(&gs_gmac_dev, gmac_dev_rx_callback);

  /* SWitch to IDLE state */
  gmac_drv_device.state = GMAC_DRV_STATE_IDLE;

  /* Notify Contiki network stack that Ethernet interface is up. */
  NETSTACK_0_MAC.connect_event(1);

  /* Driver's loop */
  while(1)
  {
    PROCESS_WAIT_EVENT();
    if (PROCESS_EVENT_EXIT == ev)
    {
      /* Process is asked to terminate */
      PRINTF("gmac-drv: exiting\n");
      /* Disable GMAC interrupts */
      INTERRUPT_DISABLE(GMAC_IRQn);
      /* TODO Clear structures and disable device */
      break;
    }
  }
  /* Set state to non-initialized */
  assert(gmac_drv_device.state != GMAC_DRV_STATE_NOT_INITIALIZED);
  gmac_drv_device.state = GMAC_DRV_STATE_NOT_INITIALIZED;
  
  /* Notify Contiki network stack that Ethernet interface is down. */
  NETSTACK_0_MAC.connect_event(0);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
const struct eth_driver gmac_driver = {
  "GMAC",
  gmac_driver_init,
  gmac_driver_reset,
  gmac_driver_set_mac_address,
  gmac_driver_get_mac_address,
  gmac_driver_tx,
  gmac_driver_is_initialized,
  gmac_driver_is_initialized,
  NULL,
  gmac_driver_exit,
};
/*---------------------------------------------------------------------------*/
#endif /* WITH_ETHERNET_SUPPORT */
