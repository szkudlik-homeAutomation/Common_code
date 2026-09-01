/*
 * tOutputStateSensor.cpp
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */
#include "../../../global.h"
#include "tOutputStateSensor.h"
#include "../tOutputProcess.h"


#if CONFIG_OUTPUT_STATE_SENSOR_JSON_OUTPUT

const char *tSensorJsonFormatter_OutputState_api_1::getSensorTypeName()
{
	static const char IdPrefix[] PROGMEM = "OutputState";
	return IdPrefix;
}

uint8_t tSensorJsonFormatter_OutputState_api_1::FormatJSON(Stream *pStream, tSensorCache *cache)
{
   tOutputStateSensorTypes::tResult_api_v1<tOutputStateSensorTypes::MAX_NUM_OF_PINS> *pResult =
		   (tOutputStateSensorTypes::tResult_api_v1<tOutputStateSensorTypes::MAX_NUM_OF_PINS> *) cache->getData();

   if (pResult->NumOfPins > tOutputStateSensorTypes::MAX_NUM_OF_PINS)
	   return STATUS_JSON_ENCODE_ERROR;

   if (cache->getDataBlobSize() != tOutputStateSensorTypes::getResultSize(pResult->NumOfPins))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   pStream->print(F("\"NumOfOutputs\":"));
   pStream->print(pResult->NumOfPins);

   uint16_t timerCorrection = cache->getTimeSinceUpdate() / 10;  // convert to seconds, as timers are in seconds

   for (uint8_t i = 0; i < pResult->NumOfPins; i++)
   {
	   pStream->print(F(","));
      pStream->print(F("\"Out_"));
      pStream->print(i);
      pStream->print(F("\":{\"State\":"));
      pStream->print((pResult->StateBitmap & ((uint16_t)1 << i)) ? 1 : 0);
      pStream->print(F(",\"Timer\":"));
      int16_t timer = pResult->Timer[i] - timerCorrection;
      if (timer < 0) 
         timer = 0;
      pStream->print(timer);
   	pStream->print(F("}"));
   }

   return STATUS_SUCCESS;
}
#endif

#if CONFIG_OUTPUT_STATE_SENSOR
tOutputStateSensor::tOutputStateSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_OUTPUT_STATES, sensorID, API_VERSION, 0, NULL)
{
   mCurrentMeasurementBlob = (void*) &mResult;
}

uint8_t tOutputStateSensor::onSetConfig()
{
	if (NULL == tOutputProcess::Instance)
		return STATUS_SENSOR_CREATE_ERROR;

	if (CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > MAX_NUM_OF_PINS)
		return STATUS_SENSOR_CREATE_ERROR;

    mResult.StateBitmap = 0;
	for (uint8_t i = 0; i < MAX_NUM_OF_PINS; i++)
	{
      mResult.Timer[i] = 0;
	}

    mResult.NumOfPins = tOutputProcess::Instance->getNumOfOutputs();

    mMeasurementBlobSize = tOutputStateSensorTypes::getResultSize(mResult.NumOfPins);
    

    return STATUS_SUCCESS;
}

void tOutputStateSensor::doTimeTick()
{
   bool changed = false;
   for (uint8_t i = 0; i < tOutputProcess::Instance->getNumOfOutputs(); i++)
   {
      uint8_t State = tOutputProcess::Instance->GetOutputState(i);
      if (((mResult.StateBitmap & ((uint16_t)1 << i)) ? 1 : 0) != State)
      {
         if (State)
            mResult.StateBitmap |= (uint16_t)1 << i;
         else
            mResult.StateBitmap &= ~((uint16_t)1 << i);
         
         changed = true;
      }

      mResult.Timer[i] = tOutputProcess::Instance->GetOutputTimer(i);
   }

   if (changed)
   {
      onMeasurementCompleted(true, EV_TYPE_MEASUREMENT_CHANGE);
   }
}

void tOutputStateSensor::doTriggerMeasurement()
{
   onMeasurementCompleted(true);
}

#endif //CONFIG_OUTPUT_STATE_SENSOR
