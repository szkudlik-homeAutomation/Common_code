/*
 * tImpulseSensor.cpp
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tImpulseSensor.h"


tImpulseSensor::tImpulseSensor() : tSensor(SENSOR_TYPE_IMPULSE)
{
	mResult.Count = 0;
	mResult.Sum = 0;
	mCnt = 0;
	mCurrentMeasurementBlob = &mResult;
	mMeasurementBlobSize = sizeof(mResult);
	mConfigSet = true;   // no config needed
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

uint8_t tImpulseSensor::TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
   if (dataBlobSize != sizeof(tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tResult *pResult =(tResult *) pDataCache;
   pStream->print(F("\"NumOfImpulses\":"));
   pStream->print(pResult->Count);
   pStream->print(F(","));
   pStream->print(F("\"SumOfImpulses\":"));
   pStream->print(pResult->Sum);
   pStream->print(F(","));

   return STATUS_SUCCESS;
}
