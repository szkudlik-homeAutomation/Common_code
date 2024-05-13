/*
 * tPt100AnalogSensor.h
 *
 *  Created on: 21 pa� 2022
 *      Author: szkud
 */

#pragma once


#include "../../../global.h"
#if CONFIG_PT100_ANALOG_SENSOR
#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorLogger.h"

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t Pt100AnalogSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache);
#endif //CONFIG_SENSORS_JSON_OUTPUT


class tPt100AnalogSensor : public tSensor {
public:
   typedef struct
   {
      uint8_t Pin;
      int8_t Correction;   // resistance of sensor cable
   } tConfig_api_v1;

   typedef struct
   {
      int Temperature;
   } tResult_api_v1;

   static const uint8_t API_VERSION = 1;
   typedef tConfig_api_v1 tConfig;
   typedef tResult_api_v1 tResult;

   tConfig Config;

   tPt100AnalogSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_PT100_ANALOG, sensorID, API_VERSION, sizeof(Config), &Config)
   {
	   mCurrentMeasurementBlob = (void*) &mResult;
	   mMeasurementBlobSize = sizeof(mResult);
	   TemperatureAvg = 0;
   }

protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();

private:
   tResult mResult;
   float TemperatureAvg;
};

class tPt100SensorLogger : public tSensorLogger
{
public:
    tPt100SensorLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_PT100_ANALOG, sensorID) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob);
};

#endif // CONFIG_PT100_ANALOG_SENSOR
