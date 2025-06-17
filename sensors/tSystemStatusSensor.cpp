/*
 * tSystemStatusSensor.cpp
 *
 *  Created on: Dec 2, 2022
 *      Author: mszkudli
 */

#include "../../../global.h"
#include "tSystemStatusSensor.h"

#if CONFIG_SYSTEM_STATUS_SENSOR_JSON_OUTPUT
uint8_t SystemStatusSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tSystemStatusSensorTypes::tResult_api_v1))
   {
		 return STATUS_JSON_ENCODE_ERROR;
   }

   tSystemStatusSensorTypes::tResult_api_v1 *pResult =
		   (tSystemStatusSensorTypes::tResult_api_v1 *) cache->getData();
   pStream->print(F("\"Uptime\":"));
   pStream->print(pResult->Uptime);
   pStream->print(F(", \"FreeMem\":"));
   pStream->print(pResult->FreeMemory);

   return STATUS_SUCCESS;
}
#endif //CONFIG_SYSTEM_STATUS_SENSOR_JSON_OUTPUT

#if CONFIG_SYSTEM_STATUS_SENSOR

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

void tSystemStatusSensorLogger::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
{
    if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
        //TODO
        return;

    tSystemStatusSensor::tResult *pResult = (tSystemStatusSensor::tResult *)pDataBlob;

    LOG_PRINT("tSystemStatusSensor SensorID: ");
    LOG(print(SensorID));
    LOG_PRINT(" FreeMemory: ");
    LOG(print(pResult->FreeMemory));
    LOG_PRINT(" Uptime: ");
    LOG(println(pResult->Uptime));
}

#endif //CONFIG_SYSTEM_STATUS_SENSOR
