/*
 * tSimpleDigitalInputSensor.cpp
 *
 *  Created on: Nov 25, 2022
 *      Author: mszkudli
 */

#include "tSimpleDigitalInputSensor.h"

uint8_t tSimpleDigitalInputSensor::TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
   if (dataBlobSize != sizeof(mResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tResult *pResult =(tResult *) pDataCache;
   pStream->print(F("\"State\":"));
   pStream->print(pResult->State,DEC);
   pStream->print(F(","));
   return STATUS_SUCCESS;
}

void tSimpleDigitalInputSensor::doTriggerMeasurement()
{
   mResult.State = (digitalRead(mConfig.Pin) == mConfig.ActiveState);

   onMeasurementCompleted(true);
}

uint8_t tSimpleDigitalInputSensor::SetSpecificConfig(void *pBlob)
{
   mConfig = *(tConfig *)pBlob;
   pinMode(mConfig.Pin, INPUT_PULLUP);

   mResult.State = 0;
   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);
   mConfigSet = true;

   return STATUS_SUCCESS;
}
