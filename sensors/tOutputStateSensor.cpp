/*
 * tOutputStateSensor.cpp
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */
#include "../../../global.h"
#if CONFIG_OUTPUT_STATE_SENSOR

#include "tOutputStateSensor.h"
#include "../tOutputProcess.h"

tOutputStateSensor::tOutputStateSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_OUTPUT_STATES, sensorID, API_VERSION, 0, NULL)
{
   for (uint8_t i = 0; i < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS; i++)
   {
      mResult.State[i] = 0;
   }

   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);
}

uint8_t tOutputStateSensor::onSetConfig()
{
	if (NULL == tOutputProcess::Instance)
		return STATUS_SENSOR_CREATE_ERROR;

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

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t OutputStateSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tOutputStateSensor::tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tOutputStateSensor::tResult *pResult =(tOutputStateSensor::tResult *) cache->getData();
   pStream->print(F("\"NumOfOutputs\":"));
   pStream->print(CONFIG_OUTPUT_PROCESS_NUM_OF_PINS);
   for (uint8_t i = 0; i < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS; i++)
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
#endif //CONFIG_SENSORS_JSON_OUTPUT
#endif //CONFIG_OUTPUT_STATE_SENSOR
