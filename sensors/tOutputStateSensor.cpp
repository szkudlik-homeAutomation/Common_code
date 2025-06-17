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
uint8_t OutputStateSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tOutputStateSensorTypes::tResult_api_v1))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tOutputStateSensorTypes::tResult_api_v1 *pResult =
		   (tOutputStateSensorTypes::tResult_api_v1 *) cache->getData();

   if (pResult->NumOfPins > tOutputStateSensorTypes::MAX_NUM_OF_PINS)
	   return STATUS_JSON_ENCODE_ERROR;

   pStream->print(F("\"NumOfOutputs\":"));
   pStream->print(pResult->NumOfPins);
   for (uint8_t i = 0; i < pResult->NumOfPins; i++)
   {
	  pStream->print(F(","));
      pStream->print(F("\"Out_"));
      pStream->print(i);
      pStream->print(F("\":{\"State\":"));
      pStream->print(pResult->State[i]);
      pStream->print(F(",\"Timer\":"));
      pStream->print(pResult->Timer[i]);
   	  pStream->print(F("}"));
   }

   return STATUS_SUCCESS;
}
#endif

#if CONFIG_OUTPUT_STATE_SENSOR
tOutputStateSensor::tOutputStateSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_OUTPUT_STATES, sensorID, API_VERSION, 0, NULL)
{
   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);
}

uint8_t tOutputStateSensor::onSetConfig()
{
	if (NULL == tOutputProcess::Instance)
		return STATUS_SENSOR_CREATE_ERROR;

	if (CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > MAX_NUM_OF_PINS)
		return STATUS_SENSOR_CREATE_ERROR;

	for (uint8_t i = 0; i < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS; i++)
	{
	     mResult.State[i] = 0;
	}

    mResult->NumOfPins = CONFIG_OUTPUT_PROCESS_NUM_OF_PINS;

    return STATUS_SUCCESS;
}

void tOutputStateSensor::doTriggerMeasurement()
{
   for (uint8_t i = 0; i < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS; i++)
   {
      mResult.State[i] = tOutputProcess::Instance->GetOutputState(i);
      mResult.Timer [i] = tOutputProcess::Instance->GetOutputTimer(i);
   }

   onMeasurementCompleted(true);
}

#endif //CONFIG_OUTPUT_STATE_SENSOR
