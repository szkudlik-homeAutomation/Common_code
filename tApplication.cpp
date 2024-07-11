/*
 * tApplication.cpp
 *
 *  Created on: 11 lip 2024
 *      Author: szkud
 */

#include "../../global.h"

#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../Common_code/WatchdogProcess.h"
#include "../Common_code/Network/network.h"
#include "../Common_code/Network/tcpServer.h"

#include "tApplication.h"

tApplication *tApplication::Instance;

void tApplication::Setup() {

#if CONFIG_LOGGER
	CONFIG_LOGGER_SERIAL.begin(CONFIG_LOGGER_TRANSPORT_SPEED);
	while (!CONFIG_LOGGER_SERIAL);
	CONFIG_LOGGER_SERIAL.print(F("START, v"));
	CONFIG_LOGGER_SERIAL.println(FW_VERSION);
#endif

	AppSetupBefore();

#if CONFIG_WATCHDOG
  WatchdogProcess.add(true);
#endif

#if CONFIG_IP_ADDRESES_FROM_EEPROM
  // NOTE! if net is not set from eeprom, it should be set in AppSetupBefore
	NetworkHwInitFromEeprom();
#endif
#if CONFIG_NETWORK
	TcpServerProcess.add(true);
#endif // CONFIG_NETWORK


#if CONFIG_TLE8457_COMM_LIB
#if TLE8457_COMM_FORCE_DEV_ID
	CommSerialSetID(TLE8457_COMM_FORCE_DEV_ID);
#elif CONFIG_TLE8457_COMM_DEV_ID_FROM_EEPROM
	CommSerialSetID(EEPROM.read(EEPROM_DEVICE_ID_OFFSET));

#endif
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


void setup() {
	if (tApplication::Instance)
		tApplication::Instance->Setup();
}
