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
#include "tSensorDesc.h"

#if CONFIG_SENSOR_HUB
class tSimpleDigitalInputSensorDesc : public tSensorDesc
{
public:
    tSimpleDigitalInputSensorDesc() : tSensorDesc() {}
    static bool isApiSupported(uint8_t apiVersion) { return (apiVersion == 1); }

protected:
#if CONFIG_SENSORS_JSON_OUTPUT
   /* sensor specific JSON formatter */
    virtual uint8_t doFormatJSON(Stream *pStream);
#endif // CONFIG_SENSORS_JSON_OUTPUT
};
#endif //CONFIG_SENSOR_HUB

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
