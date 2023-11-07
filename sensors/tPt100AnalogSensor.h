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
#include "tSensorDesc.h"

class tPt100AnalogSensorDesc : public tSensorDesc
{
public:
    tPt100AnalogSensorDesc() : tSensorDesc() {}

protected:
#if CONFIG_SENSORS_JSON_OUTPUT
   /* sensor specific JSON formatter */
    virtual uint8_t doFormatJSON(Stream *pStream);
#endif // CONFIG_SENSORS_JSON_OUTPUT
};

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

   tPt100AnalogSensor() : tSensor(SENSOR_TYPE_PT100_ANALOG, API_VERSION, sizeof(Config), &Config)
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
#endif // CONFIG_PT100_ANALOG_SENSOR
