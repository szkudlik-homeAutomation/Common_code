/*
 * tImpulseSensor.cpp
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#include "../../../global.h"

#if CONFIG_IMPULSE_SENSOR
#include "tImpulseSensor.h"

tImpulseSensor::tImpulseSensor() : tSensor(SENSOR_TYPE_IMPULSE, API_VERSION, 0, NULL)
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
uint8_t tImpulseSensorDesc::doFormatJSON(Stream *pStream)
{
    if (sensorApiVersion != 1)
    {
          return STATUS_JSON_ENCODE_UNSUPPORTED_API_VERSION;
    }

    if (dataBlobSize != sizeof(tImpulseSensor::tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tImpulseSensor::tResult *pResult =(tImpulseSensor::tResult *) pDataCache;
   pStream->print(F("\"NumOfImpulses\":"));
   pStream->print(pResult->Count);
   pStream->print(F(","));
   pStream->print(F("\"SumOfImpulses\":"));
   pStream->print(pResult->Sum);
   pStream->print(F(","));

   return STATUS_SUCCESS;
}
#endif //CONFIG_SENSORS_JSON_OUTPUT

#endif // CONFIG_IMPULSE_SENSOR
