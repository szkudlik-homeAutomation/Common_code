/*
 * tSimpleDigitalInputSensor.cpp
 *
 *  Created on: Nov 25, 2022
 *      Author: mszkudli
 */

#include "../../../global.h"
#include "tSimpleDigitalInputSensor.h"

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR_JSON_OUTPUT
uint8_t tSensorJsonFormatter_SimpleDigitalInput_api_1::FormatJSON(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tSimpleDigitalInputSensorTypes::tResult_api_v1))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tSimpleDigitalInputSensorTypes::tResult_api_v1 *pResult =
		   (tSimpleDigitalInputSensorTypes::tResult_api_v1 *) cache->getData();
   pStream->print(F("\"State\":"));
   pStream->print(pResult->State,DEC);
   return STATUS_SUCCESS;
}
#endif //CONFIG_SENSORS_JSON_OUTPUT

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR

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
