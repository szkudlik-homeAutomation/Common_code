/*
 * tPt100AnalogSensor.h
 *
 *  Created on: 21 pa� 2022
 *      Author: szkud
 */

#pragma once


#include "../../../global.h"
#if CONFIG_TGS2603_ODOUR_SENSOR
#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorLogger.h"

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t Tgs2603AnalogSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache);
#endif //CONFIG_SENSORS_JSON_OUTPUT


class tTgs2603AnalogSensor : public tSensor {
public:
   typedef struct
   {
      uint8_t Pin;
   } tConfig_api_v1;

   typedef struct
   {
      uint16_t Odour;
   } tResult_api_v1;

   static const uint8_t API_VERSION = 1;
   typedef tConfig_api_v1 tConfig;
   typedef tResult_api_v1 tResult;

   tConfig Config;

   tTgs2603AnalogSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_TGS2603, sensorID, API_VERSION, sizeof(Config), &Config)
   {
	   mCurrentMeasurementBlob = (void*) &mResult;
	   mMeasurementBlobSize = sizeof(mResult);
	   OdourAvg = 0;
   }

protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();

private:
   tResult mResult;
   float OdourAvg;
};

class tTgs2603AnalogLogger : public tSensorLogger
{
public:
	tTgs2603AnalogLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_TGS2603, sensorID) {}
	tTgs2603AnalogLogger() : tSensorLogger(SENSOR_TYPE_TGS2603, 0) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob);
};

#endif // CONFIG_TGS2603_ODOUR_SENSOR
