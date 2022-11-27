/*
 * tOutputStateSensor.h
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"
#include "tSensor.h"

class tOutputStateSensor : public tSensor {
public:
   tOutputStateSensor(uint8_t sensorID);

   typedef struct
   {
      uint8_t State[NUM_OF_OUTPUTS];
      uint16_t Timer[NUM_OF_OUTPUTS];
   } tResult;

   virtual void doTriggerMeasurement();
   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
private:
   tResult mResult;
};

