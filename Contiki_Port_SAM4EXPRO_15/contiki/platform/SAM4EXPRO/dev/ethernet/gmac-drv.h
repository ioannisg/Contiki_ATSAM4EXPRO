/*
 * gmac_drv.h
 *
 * Created: 2015-11-15 23:48:50
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */
#ifndef GMAC_DRV_H_
#define GMAC_DRV_H_

#include "dev/ethernet.h"
#include "platform-conf.h"

#ifdef GMAC_DRV_CONF_TX_PENDING_MAX_QUEUE_LEN
#define GMAC_DRV_TX_PENDING_MAX_QUEUE_LEN GMAC_DRV_CONF_TX_PENDING_MAX_QUEUE_LEN
#else
#define GMAC_DRV_TX_PENDING_MAX_QUEUE_LEN 16
#endif

/**
 * Ethernet driver states
 */
typedef enum {
  GMAC_DRV_STATE_NOT_INITIALIZED = 0x01,
  GMAC_DRV_STATE_PENDING_RESET   = 0x02,
  GMAC_DRV_STATE_IDLE            = 0x04,
  GMAC_DRV_STATE_PENDING_RX      = 0x08,
  GMAC_DRV_STATE_PENDING_TX      = 0x10,
}gmac_drv_state_t;

/** Structure for the GMAC driver context */
typedef struct {
  gmac_drv_state_t state;
  eth_stats_t stats;
} gmac_drv_t;

/** Structure for the TX pending frame meta-data list element */
struct gmac_drv_tx_pending_element {
  struct gmac_drv_tx_pending_element *next;
  linkaddr6_t addr;
};
typedef struct gmac_drv_tx_pending_element gmac_drv_tx_pending_element_t;
#endif /* GMAC_DRV_H_ */
