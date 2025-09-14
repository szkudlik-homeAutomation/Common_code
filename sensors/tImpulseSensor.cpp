/*
 * tImpulseSensor.cpp
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tImpulseSensor.h"

#if CONFIG_IMPULSE_SENSOR_JSON_OUTPUT

const char *tSensorJsonFormatter_ImpulseSensor_api_1::getSensorTypeName()
{
	static const char IdPrefix[] PROGMEM = "ImpulseCounter";
	return IdPrefix;
}

uint8_t tSensorJsonFormatter_ImpulseSensor_api_1::FormatJSON(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tImpulseSensorTypes::tResult_api_v1))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tImpulseSensorTypes::tResult_api_v1 *pResult =(tImpulseSensorTypes::tResult_api_v1 *) cache->getData();
   pStream->print(F("\"NumOfImpulses\":"));
   pStream->print(pResult->Count);
   pStream->print(F(","));
   pStream->print(F("\"SumOfImpulses\":"));
   pStream->print(pResult->Sum);

   return STATUS_SUCCESS;
}
#endif //CONFIG_IMPULSE_SENSOR_JSON_OUTPUT


#if CONFIG_IMPULSE_SENSOR

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

#endif // CONFIG_IMPULSE_SENSOR
#if CONFIG_SENSOR_LOGGER

void tImpulseSensorLogger::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob)
{
    if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
        //TODO
        return;

    tImpulseSensorTypes::tResult_api_v1 *pResult = (tImpulseSensorTypes::tResult_api_v1 *)pDataBlob;

    LOG_PRINT("SensorID: ");
    LOG(print(SensorID));
    LOG_PRINT(" impulse count: ");
    LOG(print(pResult->Count));
    LOG_PRINT(" Sum: ");
    LOG(println(pResult->Sum));
}

#endif CONFIG_SENSOR_LOGGER
