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

class tSimpleDigitalInputSensor: public tSensor {
public:
   static const uint8_t API_VERSION = 1;

   typedef struct
   {
      uint8_t Pin;
      uint8_t ActiveState;
   } tConfig;

   tConfig Config;

   typedef struct
   {
      uint8_t State;
   } tResult;

   tSimpleDigitalInputSensor() : tSensor(SENSOR_TYPE_DIGITAL_INPUT, API_VERSION) {}

   virtual void doTriggerMeasurement();

#if CONFIG_SENSORS_JSON_OUTPUT
   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
#endif CONFIG_SENSORS_JSON_OUTPUT
protected:
   virtual uint8_t doSetConfig();
private:
   tResult mResult;
};

#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
