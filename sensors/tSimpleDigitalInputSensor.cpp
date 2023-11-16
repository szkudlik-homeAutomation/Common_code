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
uint8_t tSimpleDigitalInputSensorDesc::doFormatJSON(Stream *pStream)
{
    if (sensorApiVersion != 1)
    {
          return STATUS_JSON_ENCODE_UNSUPPORTED_API_VERSION;
    }

   if (dataBlobSize != sizeof(tSimpleDigitalInputSensor::tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tSimpleDigitalInputSensor::tResult *pResult =(tSimpleDigitalInputSensor::tResult *) pDataCache;
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

uint8_t tSimpleDigitalInputSensor::onSetConfig()
{
   pinMode(Config.Pin, INPUT_PULLUP);

   mResult.State = 0;
   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);

   return STATUS_SUCCESS;
}
#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
