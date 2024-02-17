/*
 * tSystemStatusSensor.cpp
 *
 *  Created on: Dec 2, 2022
 *      Author: mszkudli
 */

#include "../../../global.h"
#if CONFIG_SYSTEM_STATUS_SENSOR

#include "tSystemStatusSensor.h"

tSystemStatusSensor::tSystemStatusSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_SYSTEM_STATUS, sensorID, API_VERSION, 0, NULL)
{
   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);
   mResult.Uptime = 0;
   tickCnt = 0;
}
void tSystemStatusSensor::doTimeTick()
{
	tickCnt++;
	if (tickCnt == (1000 / SENSOR_PROCESS_SERVICE_TIME))
	{// 1 sec
		mResult.Uptime++;
		tickCnt = 0;
	}
}

void tSystemStatusSensor::doTriggerMeasurement()
{
	mResult.FreeMemory = getFreeRam();

	onMeasurementCompleted(true);
}

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t SystemStatusSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tSystemStatusSensor::tResult))
   {
		 return STATUS_JSON_ENCODE_ERROR;
   }

   tSystemStatusSensor::tResult *pResult =(tSystemStatusSensor::tResult *) cache->getData();
   pStream->print(F("\"Uptime\":"));
   pStream->print(pResult->Uptime);
   pStream->print(F(", \"FreeMem\":"));
   pStream->print(pResult->FreeMemory);
   pStream->print(F(", \"Version\":\""));
   pStream->print(F(FW_VERSION));
   pStream->print(F("\""));

   return STATUS_SUCCESS;
}
#endif //CONFIG_SENSORS_JSON_OUTPUT
#endif //CONFIG_SYSTEM_STATUS_SENSOR
