/*
 * GlobalProcedures.cpp
 *
 *  Created on: 7 lip 2024
 *      Author: szkud
 */


#include "../../global.h"

#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"

void loop() {
  sched.run();
}


void setup() {
#if CONFIG_TLE8457_COMM_LIB
	// Initialize ID of the node

	// CommSerialSetID(CENTRAL_NODE_DEVICE_ID);
	// CommSerialSetID(EEPROM.read(EEPROM_DEVICE_ID_OFFSET));

#endif // CONFIG_TLE8457_COMM_LIB
}
