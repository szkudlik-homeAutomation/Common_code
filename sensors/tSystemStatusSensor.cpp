/*
 * tSystemStatusSensor.cpp
 *
 *  Created on: Dec 2, 2022
 *      Author: mszkudli
 */

#include "../../../global.h"
#include "tSystemStatusSensor.h"

#if CONFIG_SYSTEM_STATUS_SENSOR_JSON_OUTPUT

const char *tSensorJsonFormatter_SystemStatus_api_1::getSensorTypeName()
{
	static const char IdPrefix[] PROGMEM = "SystemStatus";
	return IdPrefix;
}

uint8_t tSensorJsonFormatter_SystemStatus_api_1::FormatJSON(Stream *pStream, tSensorCache *cache)
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

uint8_t tSensorJsonFormatter_SystemStatus_api_2::FormatJSON(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tSystemStatusSensorTypes::tResult_api_v2))
   {
		 return STATUS_JSON_ENCODE_ERROR;
   }

   tSystemStatusSensorTypes::tResult_api_v2 *pResult =
		   (tSystemStatusSensorTypes::tResult_api_v2 *) cache->getData();
   pStream->print(F("\"Uptime\":"));
   pStream->print(pResult->Uptime);
   pStream->print(F(", \"FreeMem\":"));
   pStream->print(pResult->FreeMemory);
   pStream->print(F(", \"Version\":\""));
   pStream->print(pResult->VersionMajor);
   pStream->print(F("."));
   pStream->print(pResult->VersionMinor);
   pStream->print(F("."));
   pStream->print(pResult->VersionPatch);
   pStream->print(F("\""));
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
   mResult.VersionMajor = FW_VERSION_MAJOR;
   mResult.VersionMinor = FW_VERSION_MINOR;
   mResult.VersionPatch = FW_VERSION_PATCH;
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
#endif //CONFIG_SYSTEM_STATUS_SENSOR


#if CONFIG_SENSOR_LOGGER

void tSystemStatusSensorLogger::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob)
{
    if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
        //TODO
        return;

	if (ApiVersion == 1) {
		tSystemStatusSensorTypes::tResult_api_v1 *pResult =
				(tSystemStatusSensorTypes::tResult_api_v1 *)pDataBlob;

		LOG_PRINT("tSystemStatusSensor SensorID: ");
		LOG(print(SensorID));
		LOG_PRINT(" FreeMemory: ");
		LOG(print(pResult->FreeMemory));
		LOG_PRINT(" Uptime: ");
		LOG(println(pResult->Uptime));
	}
	else if (ApiVersion == 2) {
        tSystemStatusSensorTypes::tResult_api_v2 *pResult =
                (tSystemStatusSensorTypes::tResult_api_v2 *)pDataBlob;

		LOG_PRINT("tSystemStatusSensor SensorID: ");
		LOG(print(SensorID));
		LOG_PRINT(" FreeMemory: ");
		LOG(print(pResult->FreeMemory));
		LOG_PRINT(" Uptime: ");
		LOG(print(pResult->Uptime));
		LOG_PRINT(" Version: ");
		LOG(print(pResult->VersionMajor));
		LOG(print(F(".")));
		LOG(print(pResult->VersionMinor));
		LOG(print(F(".")));
		LOG(println(pResult->VersionPatch));
	}

}

#endif
