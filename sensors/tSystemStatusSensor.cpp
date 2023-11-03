/*
 * tSystemStatusSensor.cpp
 *
 *  Created on: Dec 2, 2022
 *      Author: mszkudli
 */

#include "../../../global.h"
#if CONFIG_SYSTEM_STATUS_SENSOR

#include "tSystemStatusSensor.h"

tSystemStatusSensor::tSystemStatusSensor() : tSensor(SENSOR_TYPE_SYSTEM_STATUS, API_VERSION, 0, NULL)
{
   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);
   mResult.Uptime = 0;
}

void tSystemStatusSensor::doTriggerMeasurement()
{
	mResult.Uptime++;
	mResult.FreeMemory = getFreeRam();

	onMeasurementCompleted(true);
}

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tSystemStatusSensorDesc::doFormatJSON(Stream *pStream)
{
   if (dataBlobSize != sizeof(tSystemStatusSensor::tResult))
   {
		 return STATUS_JSON_ENCODE_ERROR;
   }

   tSystemStatusSensor::tResult *pResult =(tSystemStatusSensor::tResult *) pDataCache;
   pStream->print(F("\"Uptime\":"));
   pStream->print(pResult->Uptime);
   pStream->print(F(", \"FreeMem\":"));
   pStream->print(pResult->FreeMemory);
   pStream->print(F(", \"Version\":\""));
   pStream->print(F(FW_VERSION));
   pStream->print(F("\","));

   return STATUS_SUCCESS;
}
#endif //CONFIG_SENSORS_JSON_OUTPUT
#endif //CONFIG_SYSTEM_STATUS_SENSOR
