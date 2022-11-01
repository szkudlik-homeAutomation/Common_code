/*
 * tImpulseSensor.cpp
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tImpulseSensor.h"


tImpulseSensor::tImpulseSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_IMPULSE,sensorID)
{
	mResult.Count = 0;
	mResult.Sum = 0;
	mCnt = 0;
	mCurrentMeasurementBlob = &mResult;
	mMeasurementBlobSize = sizeof(mResult);
	mConfigSet = true;
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
   return 0;
}
