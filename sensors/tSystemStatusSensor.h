/*
 * tSystemStatusSensor.h
 *
 *  Created on: Dec 2, 2022
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"
#if CONFIG_SYSTEM_STATUS_SENSOR

#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorLogger.h"

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t SystemStatusSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache);
#endif //CONFIG_SENSORS_JSON_OUTPUT

class tSystemStatusSensor: public tSensor {
public:
   typedef struct
   {
   	   uint16_t FreeMemory;
   	   uint16_t Uptime;
   } tResult_api_v1;

   static const uint8_t API_VERSION = 1;
   typedef tResult_api_v1 tResult;

   tSystemStatusSensor(uint8_t sensorID);

protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();

private:
   tResult mResult;
   uint8_t tickCnt;
};

class tSystemStatusSensorLogger : public tSensorLogger
{
public:
	tSystemStatusSensorLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_SYSTEM_STATUS, sensorID) {}
	tSystemStatusSensorLogger() : tSensorLogger(SENSOR_TYPE_SYSTEM_STATUS, 0) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob);
};


#endif //CONFIG_SYSTEM_STATUS_SENSOR
