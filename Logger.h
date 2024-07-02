/*
 * Logger.h
 *
 *  Created on: Jan 12, 2023
 *      Author: mszkudli
 */

#pragma once

#include "../../global.h"
#if CONFIG_LOGGER

#include "helpers.h"
#include "Print.h"


#if CONFIG_LOGGER_TRANSPORT_SERIAL

#if CONFIG_LOGGER_SERIAL_PORT_1
#define CONFIG_LOGGER_SERIAL Serial
#elif CONFIG_LOGGER_SERIAL_PORT_2
#define CONFIG_LOGGER_SERIAL Serial1
#elif CONFIG_LOGGER_SERIAL_PORT_3
#define CONFIG_LOGGER_SERIAL Serial2
#elif CONFIG_LOGGER_SERIAL_PORT_4
#define CONFIG_LOGGER_SERIAL Serial3
#endif

#endif // CONFIG_LOGGER_TRANSPORT_SERIAL

class tLogger : public Print {
public:
	static tLogger *Instance;
	tLogger() : mLogsForced(false) { Instance = this; }
	virtual ~tLogger() {}

    virtual size_t write(uint8_t str);

    void EnableLogsForce() { mLogsForced = true; }
    void DisableLogsForce() { mLogsForced = false; }

private:
	  bool mLogsForced;
};

class tLogTransport
{
	friend class tLogger;
public:
	tLogTransport() :mLogEnbabled(false) { pNext = pFirst ; pFirst = this; }
	virtual ~tLogTransport();

	virtual void Log(uint8_t str)  = 0;

    void EnableLogs() { mLogEnbabled = true; }
	void DisableLogs() { mLogEnbabled = false; }

private:
	bool mLogEnbabled;
	static tLogTransport* pFirst;
	tLogTransport *pNext;
};

#if CONFIG_LOGGER_TRANSPORT_SERIAL
class tSerialLogTransport : public tLogTransport
{
public:
	static tSerialLogTransport *Instance;
	tSerialLogTransport() : tLogTransport() {Instance = this; EnableLogs();}
	virtual ~tSerialLogTransport() {};

	virtual void Log(uint8_t str) { CONFIG_LOGGER_SERIAL.write(str); }
};

#endif // CONFIG_LOGGER_TRANSPORT_SERIAL


#endif // CONFIG_LOGGER
