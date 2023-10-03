/*
 * tSystemStatusSensor.h
 *
 *  Created on: Dec 2, 2022
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"
#if CONFIG_SYSTEM_STATUS_SENSOR

#include "tSensor.h"

class tSystemStatusSensor: public tSensor {
public:
	static const uint8_t API_VERSION = 1;

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
#endif //CONFIG_SYSTEM_STATUS_SENSOR
