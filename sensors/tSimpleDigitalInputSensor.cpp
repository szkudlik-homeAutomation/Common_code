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
   onMeasurementCompleted(true);
}

void tSimpleDigitalInputSensor::doTimeTick()
{
   // detect per-bit state changes and notify only when any bit changes
   uint16_t newState = 0;

   for (uint8_t i = 0; i < Config.NumOfInputs; i++)
   {
      uint8_t polarity = (Config.ActiveStateBitmap & (1 << i)) ? 1 : 0;
      if (digitalRead(Config.Pin[i]) == polarity)
      {
         newState |= (1 << i);
      }
   }

   if (newState != mResult.State)
   {
      mResult.State = newState;
      // At least one input changed state

      onMeasurementCompleted(true, EV_TYPE_MEASUREMENT_CHANGE);
   }
}

uint8_t tSimpleDigitalInputSensor::onSetConfig()
{
   if (Config.NumOfInputs > CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR_NUM_OF_INPUTS)
   {
      return STATUS_CONFIG_SET_ERROR;
   }

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
