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
         return CREATE_SENSOR_STATUS_OTHER_ERROR;
   }

   tResult *pResult =(tResult *) pDataCache;
   pStream->print(F("\"State\":"));
   pStream->print(pResult->State,DEC);
   pStream->print(F(","));
   return CREATE_SENSOR_STATUS_OK;
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

   return CREATE_SENSOR_STATUS_OK;
}
