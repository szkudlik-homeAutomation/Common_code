/*
 * tImpulseSensor.cpp
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#include "../../../global.h"

#if CONFIG_IMPULSE_SENSOR
#include "tImpulseSensor.h"

tImpulseSensor::tImpulseSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_IMPULSE, sensorID, API_VERSION, 0, NULL)
{
	mResult.Count = 0;
	mResult.Sum = 0;
	mCnt = 0;
	mCurrentMeasurementBlob = &mResult;
	mMeasurementBlobSize = sizeof(mResult);
}

void tImpulseSensor::doTriggerMeasurement()
{
	ATOMIC
	(
		mResult.Count = mCnt;
		mResult.Sum += mCnt;
		mCnt = 0;
	);
	onMeasurementCompleted(true);
}

void tImpulseSensor::CleanSum()
{
	ATOMIC
	(
		mResult.Sum = 0;
	);
}

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t ImpulseSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tImpulseSensor::tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tImpulseSensor::tResult *pResult =(tImpulseSensor::tResult *) cache->getData();
   pStream->print(F("\"NumOfImpulses\":"));
   pStream->print(pResult->Count);
   pStream->print(F(","));
   pStream->print(F("\"SumOfImpulses\":"));
   pStream->print(pResult->Sum);

   return STATUS_SUCCESS;
}
#endif //CONFIG_SENSORS_JSON_OUTPUT

void tImpulseSensorLogger::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
{
    if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
        //TODO
        return;

    tImpulseSensor::tResult *pResult = (tImpulseSensor::tResult *)pDataBlob;

    DEBUG_PRINT_3("SensorID: ");
    DEBUG_3(print(SensorID));
    DEBUG_PRINT_3(" impulse count: ");
    DEBUG_3(print(pResult->Count));
    DEBUG_PRINT_3(" Sum: ");
    DEBUG_3(println(pResult->Sum));
}

#endif // CONFIG_IMPULSE_SENSOR
