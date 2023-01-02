/*
 * tSystemStatusSensor.h
 *
 *  Created on: Dec 2, 2022
 *      Author: mszkudli
 */

#pragma once

#define SRC_COMMON_CODE_SENSORS_TSYSTEMSTATUSSENSOR_H_

#include "../../../global.h"
#include "tSensor.h"

class tSystemStatusSensor: public tSensor {
public:
   typedef struct
   {
   	   uint16_t FreeMemory;
   	   uint16_t Uptime;
   } tResult;

   tSystemStatusSensor();

   virtual void doTriggerMeasurement();
#if CONFIG_SENSORS_JSON_OUTPUT
   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
#endif //CONFIG_SENSORS_JSON_OUTPUT

private:
   tResult mResult;
};
