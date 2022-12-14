/*
 * tSystemStatusSensor.cpp
 *
 *  Created on: Dec 2, 2022
 *      Author: mszkudli
 */

#include "tSystemStatusSensor.h"

void tSystemStatusSensor::doTriggerMeasurement()
{
	mResult.Uptime++;
	mResult.FreeMemory = getFreeRam();

	onMeasurementCompleted(true);
}

uint8_t tSystemStatusSensor::TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
   if (dataBlobSize != sizeof(tResult))
   {
		 return STATUS_JSON_ENCODE_ERROR;
   }

   tResult *pResult =(tResult *) pDataCache;
   pStream->print(F("\"Uptime\":"));
   pStream->print(pResult->Uptime);
   pStream->print(F(", \"FreeMem\":"));
   pStream->print(pResult->FreeMemory);
   pStream->print(F(","));

   return STATUS_SUCCESS;
}
