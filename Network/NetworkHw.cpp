/*
 * NetworkHw.cpp
 *
 *  Created on: 8 lip 2024
 *      Author: szkud
 */


// global should include eeprom defs
#include "../../../global.h"

#include "Network.h"

#if CONFIG_NETWORK

void NetworkHwInit(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet)
 {
	delay(3000); // hw needs some time
	Ethernet.begin(mac, ip, dns, gateway, subnet);
	while (Ethernet.hardwareStatus() == EthernetNoHardware)
		;
}
#if CONFIG_IP_ADDRESES_FROM_EEPROM
void NetworkHwInitFromEeprom()
{
	uint8_t MAC[6];
	IPAddress ip;
	IPAddress dns;
	IPAddress gateway;
	IPAddress subnet;

    for(uint8_t i= 0; i < 4; i++) ip[i] = EEPROM.read(EEPROM_IP+i);
    for(uint8_t i= 0; i < 4; i++) subnet[i] = EEPROM.read(EEPROM_IPMASK+i);
    for(uint8_t i= 0; i < 4; i++) gateway[i] = EEPROM.read(EEPROM_GATEWAY+i);
    for(uint8_t i= 0; i < 4; i++) dns[i] = EEPROM.read(EEPROM_DNS+i);
    for(uint8_t i= 0; i < 6; i++) MAC[i] = EEPROM.read(EEPROM_MAC+i);

    NetworkHwInit(MAC, ip, dns, gateway, subnet);
}


void NetworkHwSaveIpsToEeprom()
{
    for(uint8_t i= 0; i < 4; i++) EEPROM.update(EEPROM_IP+i,Ethernet.localIP[i]);
    for(uint8_t i= 0; i < 4; i++) EEPROM.update(EEPROM_IPMASK+i,Ethernet.subnetMask[i]);
    for(uint8_t i= 0; i < 4; i++) EEPROM.update(EEPROM_GATEWAY+i,Ethernet.gatewayIP[i]);
    for(uint8_t i= 0; i < 4; i++) EEPROM.update(EEPROM_DNS+i,Ethernet.dnsServerIP[i]);
    for(uint8_t i= 0; i < 6; i++) EEPROM.update(EEPROM_MAC+i,Ethernet.MACAddress[i]);
};

#endif //CONFIG_IP_ADDRESES_FROM_EEPROM

#endif  // CONFIG_NETWORK
