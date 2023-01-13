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

class tPt100AnalogSensor : public tSensor {
public:
   typedef struct
   {
      uint8_t Pin;
      int8_t Correction;   // resistance of sensor cable
   } tConfig;

   typedef struct
   {
      int Temperature;
   } tResult;

   tPt100AnalogSensor() : tSensor(SENSOR_TYPE_PT100_ANALOG) {}

   virtual uint8_t SetSpecificConfig(void *pBlob);

#if CONFIG_SENSORS_JSON_OUTPUT
   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
#endif CONFIG_SENSORS_JSON_OUTPUT

protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();
private:
   tResult mResult;
   tConfig mConfig;
   float TemperatureAvg;
};
#endif // CONFIG_PT100_ANALOG_SENSOR
