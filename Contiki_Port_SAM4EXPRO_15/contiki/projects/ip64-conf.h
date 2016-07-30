/*
 * ip64_conf.h
 *
 * Created: 2016-07-29 12:54:33
 *  Author: ioannisg
 */ 


#ifndef IP64_CONF_H_
#define IP64_CONF_H_

//#include "ip64-tap-driver.h"
#include "ip64-eth-interface.h"
#include "ip64-null-driver.h"

#define IP64_CONF_UIP_FALLBACK_INTERFACE    ip64_eth_interface
#define IP64_CONF_INPUT                     ip64_eth_interface_input

extern const struct eth_driver gmac_driver;

#define IP64_CONF_ETH_DRIVER                ip64_null_driver



#endif /* IP64-CONF_H_ */