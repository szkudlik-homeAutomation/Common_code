/*
 * tApplication.cpp
 *
 *  Created on: 11 lip 2024
 *      Author: szkud
 */

#include "../../global.h"

#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "WatchdogProcess.h"
#include "Network/network.h"
#include "Network/tcpServer.h"
#include "sensors/tSensor.h"
#include "tOutputProcess.h"
#include "tInputProcess.h"
#include "TLE8457_serial/TLE8457_serial_lib.h"
#include "tWorkerProcess.h"
#include "tApplication.h"
#include "Network/httpServer.h"
#include "sensors/tSensorHubMessageReciever.h"
#include "sensors/tSensorControlFromRemote.h"
#include "WatchdogProcess.h"

#if CONFIG_WORKER_PROCESS
tWorkerProcess WorkerProcess;
#endif

#if CONFIG_TLE8457_COMM_LIB
// instantiate the process
CommRecieverProcess CommReciever;
CommSenderProcess CommSenderProcess;
#endif // CONFIG_TLE8457_COMM_LIB


#if CONFIG_HTTP_SERVER_INSTANCE
tHttpServer HttpServer;
#endif //CONFIG_HTTP_SERVER_INSTANCE



#if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
tSensorHubMessageReciever SensorHubMessageReciever;
#endif //CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS


#if CONFIG_WATCHDOG
tWatchdogProcess WatchdogProcess;
#endif //CONFIG_WATCHDOG

#if CONFIG_SENSORS
tSensorProcess SensorProcess;
#endif //CONFIG_SENSORS

#if CONFIG_SENSOR_BASIC_REMOTE_CONTROL
tSensorControlFromRemote SensorControlFromRemote;
#endif CONFIG_SENSOR_BASIC_REMOTE_CONTROL


tApplication *tApplication::Instance;

void tApplication::Setup() {

#if CONFIG_LOGGER_TRANSPORT_SERIAL
	CONFIG_LOGGER_SERIAL.begin(CONFIG_LOGGER_TRANSPORT_SPEED);
	while (!CONFIG_LOGGER_SERIAL);
	CONFIG_LOGGER_SERIAL.print(F("START, v"));
	CONFIG_LOGGER_SERIAL.println(FW_VERSION);
#endif

    DEBUG_PRINT_1("AppSetupBefore...");
	AppSetupBefore();
	DEBUG_PRINTLN_1("...done");

#if CONFIG_WATCHDOG
  DEBUG_PRINT_1("WatchdogProcess...");
  WatchdogProcess.add(true);
  DEBUG_PRINTLN_1("...done");
#endif

#if CONFIG_IP_ADDRESES_FROM_EEPROM
  // NOTE! if net is not set from eeprom, it should be set in AppSetupBefore
    DEBUG_PRINT_1("NetworkHwInitFromEeprom...");
	NetworkHwInitFromEeprom();
    DEBUG_PRINTLN_1("...done");
#endif
#if CONFIG_NETWORK
    DEBUG_PRINT_1("TcpServerProcess...");
	TcpServerProcess.add(true);
    DEBUG_PRINTLN_1("...done");
#endif // CONFIG_NETWORK
#if CONFIG_SENSORS
    DEBUG_PRINT_1("tSensorProcess...");
    if (tSensorProcess::Instance)
    	tSensorProcess::Instance->add(true);
    else
    	DEBUG_PRINTLN_3("!!!! No tSensorProcess::Instance!");

    DEBUG_PRINTLN_1("...done");
#endif // CONFIG_SENSORS

#if CONFIG_OUTPUT_PROCESS
    DEBUG_PRINT_1("tOutputProcess...");
    if (tOutputProcess::Instance)
    	tOutputProcess::Instance->add(true);
    else
    	DEBUG_PRINTLN_3("!!!! No tOutputProcess::Instance!");

    DEBUG_PRINTLN_1("...done");
#endif // CONFIG_OUTPUT_PROCESS

#if CONFIG_INPUT_PROCESS
    DEBUG_PRINT_1("tInputProcess...");
    if (tInputProcess::Instance)
    	tInputProcess::Instance->add(true);
    else
    	DEBUG_PRINTLN_3("!!!! No tInputProcess::Instance!");

    DEBUG_PRINTLN_1("...done");
#endif // CONFIG_INPUT_PROCESS

#if CONFIG_TLE8457_COMM_LIB
      DEBUG_PRINT_1("TLE8457_COMM...");
	  COMM_SERIAL.begin(CONFIG_TRANSMISSION_SPEED);
	  while (!COMM_SERIAL);

      DEBUG_PRINT_1("CommSenderProcess...");
	  CommSenderProcess::Instance->add();
      DEBUG_PRINT_1("CommRecieverProcess...");
	  CommRecieverProcess::Instance->add();
#if CONFIG_TLE8457_COMM_FORCE_DEV_ID
	CommSerialSetID(CONFIG_TLE8457_COMM_FORCE_DEV_ID);
#elif CONFIG_TLE8457_COMM_DEV_ID_FROM_EEPROM
	CommSerialSetID(EEPROM.read(EEPROM_DEVICE_ID_OFFSET));
#endif
    DEBUG_PRINTLN_1("...done");
#endif // CONFIG_TLE8457_COMM_LIB

#if CONFIG_WORKER_PROCESS
    DEBUG_PRINT_1("tWorkerProcess...");
	tWorkerProcess::Instance->add();
    DEBUG_PRINTLN_1("...done");
#endif // CONFIG_WORKER_PROCESS


#if CONFIG_SENSORS_RESTORE_FROM_EEPROM_AT_STARTUP
    tSensor::RestoreFromEEprom();
#endif CONFIG_SENSORS_RESTORE_FROM_EEPROM_AT_STARTUP

    DEBUG_PRINT_1("AppSetupAfter...");
	AppSetupAfter();
    DEBUG_PRINTLN_1("...done");

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
