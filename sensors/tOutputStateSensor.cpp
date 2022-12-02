/*
 * tOutputStateSensor.cpp
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#include "tOutputStateSensor.h"
#include "../OutputProcess.h"

tOutputStateSensor::tOutputStateSensor()  : tSensor(SENSOR_TYPE_OUTPUT_STATES)
{
   for (uint8_t i = 0; i < NUM_OF_OUTPUTS; i++)
   {
      mResult.State[i] = 0;
   }

   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);
   mConfigSet = true;
}

extern tOutputProcess OutputProcess;

void tOutputStateSensor::doTriggerMeasurement()
{
   for (uint8_t i = 0; i < NUM_OF_OUTPUTS; i++)
   {
      mResult.State[i] = OutputProcess.GetOutputState(i);
      mResult.Timer [i] = OutputProcess.GetOutputTimer(i);
   }

   onMeasurementCompleted(true);
}

uint8_t tOutputStateSensor::TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
   if (dataBlobSize != sizeof(tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tResult *pResult =(tResult *) pDataCache;
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
