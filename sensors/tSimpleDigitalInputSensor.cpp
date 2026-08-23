/*
 * tSimpleDigitalInputSensor.cpp
 *
 *  Created on: Nov 25, 2022
 *      Author: mszkudli
 */

#include "../../../global.h"
#include "tSimpleDigitalInputSensor.h"

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR_JSON_OUTPUT

const char *tSensorJsonFormatter_SimpleDigitalInput_api_1::getSensorTypeName()
{
	static const char IdPrefix[] PROGMEM = "DigitalInput";
	return IdPrefix;
}

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

uint8_t tSensorJsonFormatter_SimpleDigitalInput_api_2::FormatJSON(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tSimpleDigitalInputSensorTypes::tResult_api_v2))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }
   tSimpleDigitalInputSensorTypes::tResult_api_v2 *pResult =
		   (tSimpleDigitalInputSensorTypes::tResult_api_v2 *) cache->getData();

   pStream->print(F("\"NumOfInputs\":"));
   pStream->print(pResult->NumOfInputs);
   if (pResult->NumOfInputs == 1)
   {
      // for backward compatibility, if only one input, provide State as well
       pStream->print(F(",\"State\":"));
       pStream->print(pResult->State,DEC);
   }
   for (uint8_t i = 0; i < pResult->NumOfInputs; i++)
   {
      pStream->print(F(","));
      pStream->print(F("\"Input_"));
      pStream->print(i);
      pStream->print(F("\":{\"State\":"));
      pStream->print((pResult->State & (1 << i)) ? 1 : 0,DEC);
      pStream->print(F("}"));
   }
   return STATUS_SUCCESS;
}

#endif //CONFIG_SENSORS_JSON_OUTPUT

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR

void tSimpleDigitalInputSensor::doTriggerMeasurement()
{
   mResult.State = 0;
   for (uint8_t i=0; i<Config.NumOfInputs; i++)
   {
      if (digitalRead(Config.Pin[i]) == ((Config.ActiveStateBitmap & (1 << i)) ? 1 : 0))
      {
         mResult.State |= (1 << i);
      }
   }

   onMeasurementCompleted(true);
}

uint8_t tSimpleDigitalInputSensor::onSetConfig()
{
   for (uint8_t i=0; i<Config.NumOfInputs; i++)
   {
      pinMode(Config.Pin[i], INPUT_PULLUP);
   }

   mResult.State = 0;
   mResult.NumOfInputs = Config.NumOfInputs;
   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);

   return STATUS_SUCCESS;
}
#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
