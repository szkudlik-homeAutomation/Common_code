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

class tLogger : public Print {
public:
	tLogger() {}
	virtual ~tLogger() {}

    virtual size_t write(uint8_t str);

};

extern tLogger Logger;

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

#ifdef DEBUG_SERIAL
class tSerialLogTransport : public tLogTransport
{
public:
	tSerialLogTransport() : tLogTransport() {EnableLogs();}
	virtual ~tSerialLogTransport() {};

	virtual void Log(uint8_t str) { DEBUG_SERIAL.write(str); }
};

extern tSerialLogTransport SerialLogTransport;

#endif


#endif // CONFIG_LOGGER
