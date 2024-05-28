/*
 * tImpulseSensor.cpp
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#include "../../../global.h"

#if CONFIG_IMPULSE_SENSOR
#include "tImpulseSensor.h"


volatile uint16_t tImpulseSensor::mCnt[3];
uint8_t tImpulseSensor::mLastSensorNum  = 0;

// IMPULSE_SENSOR_MAX_SENSORS
void tImpulseSensor::Impulse0()
{
	mCnt[0]++;
}

void tImpulseSensor::Impulse1()
{
	mCnt[1]++;
}

void tImpulseSensor::Impulse2()
{
	mCnt[2]++;
}


uint8_t tImpulseSensor::onSetConfig()
{
	pinMode(Config.Pin, INPUT_PULLUP);
	switch (mSensorNum)
	{
		case 0: attachInterrupt(digitalPinToInterrupt(Config.Pin), Impulse0, FALLING); break;
		case 1: attachInterrupt(digitalPinToInterrupt(Config.Pin), Impulse1, FALLING); break;
		case 2: attachInterrupt(digitalPinToInterrupt(Config.Pin), Impulse2, FALLING); break;
		default: return STATUS_SENSOR_CREATE_ERROR;
	}
	return STATUS_SUCCESS;
}

tImpulseSensor::tImpulseSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_IMPULSE, sensorID, API_VERSION, sizeof(Config), &Config)
{
	mSensorNum = mLastSensorNum++;
	mResult.Count = 0;
	mResult.Sum = 0;
	mCnt[mSensorNum] = 0;
	mCurrentMeasurementBlob = &mResult;
	mMeasurementBlobSize = sizeof(mResult);
}

void tImpulseSensor::doTriggerMeasurement()
{
	ATOMIC
	(
		mResult.Count = mCnt[mSensorNum];
		mResult.Sum += mCnt[mSensorNum];
		mCnt[mSensorNum] = 0;
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
