/*
 * GlobalProcedures.cpp
 *
 *  Created on: 7 lip 2024
 *      Author: szkud
 */


#include "../../global.h"

#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../Common_code/WatchdogProcess.h"


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

#if CONFIG_TLE8457_COMM_LIB
	// Initialize ID of the node

	// CommSerialSetID(CENTRAL_NODE_DEVICE_ID);
	// CommSerialSetID(EEPROM.read(EEPROM_DEVICE_ID_OFFSET));

#endif // CONFIG_TLE8457_COMM_LIB
}


void loop() {
  sched.run();
}
