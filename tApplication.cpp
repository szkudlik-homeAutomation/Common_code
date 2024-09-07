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
#include "TLE8457_serial/TLE8457_serial_lib.h"
#include "tWorkerProcess.h"
#include "tApplication.h"
#include "Network/httpServer.h"
#include "sensors/tSensorHubMessageReciever.h"
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



#if CONFIG_SENSOR_HUB_MESSAGE_RECIEVER
tSensorHubMessageReciever SensorHubMessageReciever;
#endif //CONFIG_SENSOR_HUB_MESSAGE_RECIEVER


#if CONFIG_WATCHDOG
tWatchdogProcess WatchdogProcess;
#endif //CONFIG_WATCHDOG

#if CONFIG_SENSORS
tSensorProcess SensorProcess;
#endif //CONFIG_SENSORS


tApplication *tApplication::Instance;

void tApplication::Setup() {

#if CONFIG_LOGGER_TRANSPORT_SERIAL
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
#if CONFIG_SENSORS
	tSensorProcess::Instance->add(true);
#endif // CONFIG_SENSORS

#if CONFIG_OUTPUT_PROCESS_INSTANCE
	tOutputProcess::Instance->add(true);
#endif // CONFIG_OUTPUT_PROCESS_INSTANCE

#if CONFIG_TLE8457_COMM_LIB
	  COMM_SERIAL.begin(CONFIG_TRANSMISSION_SPEED);
	  while (!COMM_SERIAL);

	  CommSenderProcess::Instance->add();
	  CommRecieverProcess::Instance->add();
#if CONFIG_TLE8457_COMM_FORCE_DEV_ID
	CommSerialSetID(CONFIG_TLE8457_COMM_FORCE_DEV_ID);
#elif CONFIG_TLE8457_COMM_DEV_ID_FROM_EEPROM
	CommSerialSetID(EEPROM.read(EEPROM_DEVICE_ID_OFFSET));
#endif
#endif // CONFIG_TLE8457_COMM_LIB

#if CONFIG_WORKER_PROCESS
	tWorkerProcess::Instance->add();
#endif // CONFIG_WORKER_PROCESS

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
