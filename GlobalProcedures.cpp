/*
 * GlobalProcedures.cpp
 *
 *  Created on: 7 lip 2024
 *      Author: szkud
 */


#include "../../global.h"

#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../Common_code/WatchdogProcess.h"
#include "../Common_code/Network/network.h"
#include "../Common_code/Network/tcpServer.h"



__attribute__((weak)) void AppSetupBefore() {}
__attribute__((weak)) void AppSetupAfter() {}


void setup() {

#if CONFIG_WATCHDOG
  WatchdogProcess.add(true);
#endif

#if CONFIG_LOGGER
  CONFIG_LOGGER_SERIAL.begin(115200);
  while (!CONFIG_LOGGER_SERIAL);
  CONFIG_LOGGER_SERIAL.print(F("START, v"));
  CONFIG_LOGGER_SERIAL.println(FW_VERSION);
#endif




  AppSetupBefore();





#if CONFIG_IP_ADDRESES_FROM_EEPROM
  // NOTE! if net is not set from eeprom, it should be set in AppSetupBefore
	NetworkHwInitFromEeprom();
#endif
#if CONFIG_NETWORK
	TcpServerProcess.add(true);
#endif // CONFIG_NETWORK







#if CONFIG_TLE8457_COMM_LIB
	// Initialize ID of the node

	// CommSerialSetID(CENTRAL_NODE_DEVICE_ID);
	// CommSerialSetID(EEPROM.read(EEPROM_DEVICE_ID_OFFSET));

#endif // CONFIG_TLE8457_COMM_LIB

	AppSetupAfter();

#ifdef CONFIG_LOGGER_SERIAL
  CONFIG_LOGGER_SERIAL.print(F("Free RAM: "));
  CONFIG_LOGGER_SERIAL.println(getFreeRam());
  CONFIG_LOGGER_SERIAL.println(F("SYSTEM INITIALIZED"));
#endif

}


void loop() {
  sched.run();
}
