/*
 * tPt100AnalogSensor.h
 *
 *  Created on: 21 paü 2022
 *      Author: szkud
 */

#pragma once


#include "../../../global.h"
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

   tPt100AnalogSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_PT100_ANALOG,sensorID) {}

   virtual uint8_t SetSpecificConfig(void *pBlob);

   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();
private:
   tResult mResult;
   tConfig mConfig;
   float TemperatureAvg;
};
