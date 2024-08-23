#pragma once

#include "../../../global.h"

#if CONFIG_NETWORK

#include <SPI.h>

#include "../../lib/Ethernet/src/localEthernet.h"

void NetworkHwInit(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);

#if CONFIG_IP_ADDRESES_FROM_EEPROM

/*
 *     	requires symbols (eeprom offset):
 *    	EEPROM_IP, EEPROM_IPMASK, EEPROM_GATEWAY, EEPROM_DNS, EEPROM_MAC
 *
 */
void NetworkHwInitFromEeprom();
void NetworkHwSaveIpsToEeprom();

#endif //CONFIG_IP_ADDRESES_FROM_EEPROM

#endif // CONFIG_NETWORK
