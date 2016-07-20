/*
 * ieee8023_mac.h
 *
 * Created: 5/2/2014 5:14:55 PM
 *  Author: ioannisg
 */ 


#ifndef IEEE8023_MAC_H_
#define IEEE8023_MAC_H_

#include "net/mac/mac.h"
#include "contiki-conf.h"

#ifdef IEEE8023_CONF_MAC_ADDRESS_FILTER
#define IEEE8023_MAC_ADDRESS_FILTER IEEE8023_CONF_MAC_ADDRESS_FILTER
#else
#define IEEE8023_MAC_ADDRESS_FILTER 0
#endif

extern const struct mac_driver ieee8023_mac_driver;

#endif /* IEEE8023_MAC_H_ */