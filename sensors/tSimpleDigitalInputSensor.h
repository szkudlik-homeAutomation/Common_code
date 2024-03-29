/*
 * tSimpleDigitalInputSensor.h
 *
 *  Created on: Nov 25, 2022
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"
#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR

#include "tSensor.h"
#include "tSensorCache.h"

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t SimpleDigitalInputSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache);
#endif //CONFIG_SENSORS_JSON_OUTPUT

class tSimpleDigitalInputSensor: public tSensor {
public:
   typedef struct
   {
      uint8_t Pin;
      uint8_t ActiveState;
   } tConfig_api_v1;

   typedef struct
   {
      uint8_t State;
   } tResult_api_v1;

   static const uint8_t API_VERSION = 1;
   typedef tConfig_api_v1 tConfig;
   typedef tResult_api_v1 tResult;

   tConfig Config;

   tSimpleDigitalInputSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_DIGITAL_INPUT, sensorID, API_VERSION, sizeof(Config), &Config) {}

   virtual void doTriggerMeasurement();

protected:
   virtual uint8_t onSetConfig();
private:
   tResult mResult;
};

#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
