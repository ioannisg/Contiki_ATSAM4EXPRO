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

// TODO Move all this to core/dev/ethernet.h
typedef enum {
  GMAC_DRV_STATE_NOT_INITIALIZED = 0x01,
  GMAC_DRV_STATE_PENDING_RESET   = 0x02,
  GMAC_DRV_STATE_IDLE            = 0x04,
  GMAC_DRV_STATE_PENDING_RX      = 0x08,
  GMAC_DRV_STATE_PENDING_TX      = 0x10,
}gmac_drv_state_t;

typedef struct {
  uint32_t tx;
  uint32_t rx;
  uint32_t tx_err;
  uint32_t rx_err_no_buf;
  uint32_t rx_err_overrun;
} gmac_drv_stats_t;

typedef struct {
  gmac_drv_state_t state;
  gmac_drv_stats_t stats;
} gmac_drv_t;

#endif /* GMAC_DRV_H_ */