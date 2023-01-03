/*
 * tSimpleDigitalInputSensor.cpp
 *
 *  Created on: Nov 25, 2022
 *      Author: mszkudli
 */

#include "../../../global.h"
#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR

#include "tSimpleDigitalInputSensor.h"

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tSimpleDigitalInputSensor::TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
   if (dataBlobSize != sizeof(tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tResult *pResult =(tResult *) pDataCache;
   pStream->print(F("\"State\":"));
   pStream->print(pResult->State,DEC);
   pStream->print(F(","));
   return STATUS_SUCCESS;
}
#endif //CONFIG_SENSORS_JSON_OUTPUT

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
#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
