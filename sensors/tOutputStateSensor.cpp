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

tOutputStateSensor::tOutputStateSensor() : tSensor(SENSOR_TYPE_OUTPUT_STATES, API_VERSION, 0, NULL)
{
   for (uint8_t i = 0; i < NUM_OF_OUTPUTS; i++)
   {
      mResult.State[i] = 0;
   }

   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);
}

void tOutputStateSensor::doTriggerMeasurement()
{
   for (uint8_t i = 0; i < NUM_OF_OUTPUTS; i++)
   {
      mResult.State[i] = tOutputProcess::get()->GetOutputState(i);
      mResult.Timer [i] = tOutputProcess::get()->GetOutputTimer(i);
   }

   onMeasurementCompleted(true);
}

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tOutputStateSensorDesc::doFormatJSON(Stream *pStream)
{
   if (dataBlobSize != sizeof(tOutputStateSensor::tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tOutputStateSensor::tResult *pResult =(tOutputStateSensor::tResult *) pDataCache;
   pStream->print(F("\"NumOfOutputs\":"));
   pStream->print(NUM_OF_OUTPUTS);
   pStream->print(F(","));
   for (uint8_t i = 0; i < NUM_OF_OUTPUTS; i++)
   {
//      "Out_1": {"State":0, "Timer":1 },

      pStream->print(F("\"Out_"));
      pStream->print(i);
      pStream->print(F("\":{\"State\":"));
      pStream->print(pResult->State[i]);
      pStream->print(F(",\"Timer\":"));
      pStream->print(pResult->Timer[i]);
      pStream->print(F("},"));
   }

   return STATUS_SUCCESS;
}
#endif //CONFIG_SENSORS_JSON_OUTPUT
#endif //CONFIG_OUTPUT_STATE_SENSOR
