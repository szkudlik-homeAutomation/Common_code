/*
 * tSystemStatusSensor.h
 *
 *  Created on: Dec 2, 2022
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"
#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorLogger.h"

#if CONFIG_SYSTEM_STATUS_SENSOR_JSON_OUTPUT
class tSensorJsonFormatter_SystemStatus_api_1 : public tSensorJsonFormatter
{
public:
	tSensorJsonFormatter_SystemStatus_api_1() : tSensorJsonFormatter() {}
protected:
	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) override;
};

class tSensorJsonFormatter_SystemStatus_api_2 : public tSensorJsonFormatter
{
public:
	tSensorJsonFormatter_SystemStatus_api_2() : tSensorJsonFormatter() {}
protected:
	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) override;
};

#endif //CONFIG_SYSTEM_STATUS_SENSOR_JSON_OUTPUT

#if CONFIG_SYSTEM_STATUS_SENSOR || CONFIG_SYSTEM_STATUS_SENSOR_JSON_OUTPUT || CONFIG_SENSOR_LOGGER
class tSystemStatusSensorTypes
{
public:
	   typedef struct
	   {
	   	   uint16_t FreeMemory;
	   	   uint16_t Uptime;
	   } tResult_api_v1;

	   typedef struct
	   {
	   	   uint16_t FreeMemory;
	   	   uint16_t Uptime;
	   	   uint8_t  VersionMajor;
	   	   uint8_t  VersionMinor;
	   	   uint8_t  VersionPatch;
	   } tResult_api_v2;

};
#endif

#if CONFIG_SENSOR_LOGGER
class tSystemStatusSensorLogger : public tSensorLogger
{
public:
	tSystemStatusSensorLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_SYSTEM_STATUS, sensorID) {}
	tSystemStatusSensorLogger() : tSensorLogger(SENSOR_TYPE_SYSTEM_STATUS, 0) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob) override;
};
#endif


#if CONFIG_SYSTEM_STATUS_SENSOR
class tSystemStatusSensor: public tSensor, public tSystemStatusSensorTypes {
public:
   static const uint8_t API_VERSION = 2;
   typedef tResult_api_v2 tResult;

   tSystemStatusSensor(uint8_t sensorID);

protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();

private:
   tResult mResult;
   uint8_t tickCnt;
};


#endif //CONFIG_SYSTEM_STATUS_SENSOR
