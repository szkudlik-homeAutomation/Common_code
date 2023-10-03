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
   mResult.State = (digitalRead(Config.Pin) == Config.ActiveState);

   onMeasurementCompleted(true);
}

uint8_t tSimpleDigitalInputSensor::doSetConfig()
{
   pinMode(Config.Pin, INPUT_PULLUP);

   mResult.State = 0;
   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);

   return STATUS_SUCCESS;
}
#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
