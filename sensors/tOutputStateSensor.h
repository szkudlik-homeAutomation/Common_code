/*
 * tOutputStateSensor.h
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"
#if CONFIG_OUTPUT_STATE_SENSOR
#include "tSensor.h"

class tOutputStateSensor : public tSensor {
public:
   static const uint8_t API_VERSION = 1;

   tOutputStateSensor();

   typedef struct
   {
      uint8_t State[NUM_OF_OUTPUTS];
      uint16_t Timer[NUM_OF_OUTPUTS];
   } tResult;

#if CONFIG_SENSORS_JSON_OUTPUT
   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
#endif //CONFIG_SENSORS_JSON_OUTPUT

protected:
   virtual void doTriggerMeasurement();

private:
   tResult mResult;
};

#endif //CONFIG_OUTPUT_STATE_SENSOR
