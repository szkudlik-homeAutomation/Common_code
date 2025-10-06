
/*
 * tDS1820Sensor.cpp
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tDS1820Sensor.h"
#include "../../lib/OneWire/OneWire.h"
#include "../../lib/ds1820/DallasTemperature.h"


#if CONFIG_DS1820_SENSOR_JSON_OUTPUT

const char *tSensorJsonFormatter_DS1820_api_1::getSensorTypeName()
{
	static const char IdPrefix[] PROGMEM = "DS1820";
	return IdPrefix;
}

uint8_t tSensorJsonFormatter_DS1820_api_1::verifyConsistency(tSensorCache *cache)
{
	   if (cache->getDataBlobSize() < sizeof(tDS1820SensorTypes::tResult_api_v1))
	   {
	         return STATUS_JSON_ENCODE_ERROR;
	   }

	   tDS1820SensorTypes::tResult_api_v1 *pResult =(tDS1820SensorTypes::tResult_api_v1 *) cache->getData();
	   uint8_t MeasurementBlobSize = sizeof(tDS1820SensorTypes::tResult_api_v1) +
	         (sizeof(tDS1820SensorTypes::tDs1820Data) * pResult->NumOfDevices);
	   if (cache->getDataBlobSize() != MeasurementBlobSize)
	   {
	         return STATUS_JSON_ENCODE_ERROR;
	   }

	   return STATUS_SUCCESS;
}

uint8_t tSensorJsonFormatter_DS1820_api_1::FormatJSON(Stream *pStream, tSensorCache *cache)
{
	uint8_t Status = verifyConsistency(cache);

	if (Status != STATUS_SUCCESS)
		return Status;

   tDS1820SensorTypes::tResult_api_v1 *pResult =(tDS1820SensorTypes::tResult_api_v1 *) cache->getData();

   pStream->print(F("\"NumOfDevs\":"));
   pStream->print(pResult->NumOfDevices);
   pStream->print(F(",\"Avg\":"));
   pStream->print(pResult->Avg);
   if (pResult->Avg)
   {
      pStream->print(F(",\"AvgTemperature\":"));
      pStream->print((float)pResult->Dev[0].Temperature / 10);
   }
   else
   {
       pStream->print(F(","));
	   _formatJSON(pStream, cache);
   }

   return STATUS_SUCCESS;
}

void tSensorJsonFormatter_DS1820_api_1::_formatJSON(Stream *pStream, tSensorCache *cache)
{
	tDS1820SensorTypes::tResult_api_v1 *pResult =(tDS1820SensorTypes::tResult_api_v1 *) cache->getData();

    for (uint8_t i = 0; i < pResult->NumOfDevices; i++)
    {
       pStream->print(F("\"Temperature_"));
       tDS1820SensorTypes::printAddress((uint8_t*)&pResult->Dev[i].Addr,pStream);
       pStream->print(F("\":"));
       pStream->print((float)pResult->Dev[i].Temperature / 10);
       if (i < (pResult->NumOfDevices - 1))
       {
          pStream->print(F(","));
       }
    }
}

#if CONFIG_DS1820_SENSOR_AGGREAGETED_JSON_OUTPUT
uint8_t tSensorJsonFormatter_DS1820_api_1::formatJSONAggregate(Stream *pStream, tSensorCache *cache)
{
	uint8_t Status = verifyConsistency(cache);

	if (Status != STATUS_SUCCESS)
		return Status;

	_formatJSON(pStream, cache);

	return STATUS_SUCCESS;
}
#endif //CONFIG_DS1820_SENSOR_AGGREAGETED_JSON_OUTPUT

void tDS1820SensorTypes::printAddress(uint8_t* pDeviceAddress, Stream *pStream)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (pDeviceAddress[i] < 16) pStream->print("0");
    pStream->print(pDeviceAddress[i], HEX);
  }
}

#endif CONFIG_DS1820_SENSOR_JSON_OUTPUT

#if CONFIG_DS1820_SENSOR

uint8_t tDS1820Sensor::onSetConfig()
{
   OneWire * pOneWire = new OneWire(Config.Pin);
   pDs1820 = new DallasTemperature(pOneWire);
   pDs1820->begin();
   pDs1820->setWaitForConversion(false);
   mNumOfDevices = pDs1820->getDeviceCount();
   if (mNumOfDevices > MAX_DS1820_DEVICES_ON_BUS)
      mNumOfDevices  = MAX_DS1820_DEVICES_ON_BUS;

   // calculate size of the result
   mMeasurementBlobSize = sizeof(tResult) + (sizeof(tDs1820Data) * mNumOfDevices);
   mCurrentMeasurementBlob = malloc(mMeasurementBlobSize);

   getCurrentMeasurement()->Avg = Config.Avg;
   getCurrentMeasurement()->NumOfDevices = mNumOfDevices;

   if (!Config.Avg)
   {
      // set device IDs
      for (uint8_t i = 0; i < mNumOfDevices; i++)
      {
         DEBUG_PRINT_3("tSensor - looking for device ");
         DEBUG_3(println(i));
         bool DevExist = pDs1820->getAddress(getCurrentMeasurement()->Dev[i].Addr,i);
         if (! DevExist)
         {
            DEBUG_PRINTLN_3("            ERROR device not found");
            return STATUS_CONFIG_SET_ERROR;
         }
#if CONFIG_LOGLEVEL_3
         else
         {
            DEBUG_PRINT_3("             FOUND, serial: ");
            // zonk, response_handeler is not a stream... so serial only
            printAddress((uint8_t*)getCurrentMeasurement()->Dev[i].Addr, &CONFIG_LOGGER_SERIAL);
            DEBUG_PRINTLN_3("");
         }
#endif
      }
   }
   return STATUS_SUCCESS;
}

void tDS1820Sensor::doTriggerMeasurement()
{
   pDs1820->requestTemperatures();
   mTicksToMeasurementCompete = NUM_TICKS_TO_MEASURE_COMPETE;
}


void tDS1820Sensor::doTimeTick()
{
   if (mTicksToMeasurementCompete)
   {
      bool Success = true;

      mTicksToMeasurementCompete--;
      if (0 == mTicksToMeasurementCompete)
      {
         if (mNumOfDevices != pDs1820->getDS18Count())
         {
            Success = false;
         }
         else if (Config.Avg)
         {
            getCurrentMeasurement()->Dev[0].Temperature = 0;
            uint8_t NumOfValidMeasurements = 0;
            for (uint8_t i = 0; i < mNumOfDevices ; i++)
            {
               int16_t temp = round(pDs1820->getTempCByIndex(i) * 10);
               if (isTempValid(temp))
               {
                  getCurrentMeasurement()->Dev[0].Temperature += temp;
                  NumOfValidMeasurements++;
               }
               else
               {
                  Success = false;  // error
               }
            }
            getCurrentMeasurement()->Dev[0].Temperature /= NumOfValidMeasurements;
         }
         else  // is !Config.Avg
         {
            Success = true;
            for (uint8_t i = 0; i < mNumOfDevices ; i++)
            {
               int16_t temp = round(pDs1820->getTempCByIndex(i) * 10);
               if (isTempValid(temp))
               {
                  getCurrentMeasurement()->Dev[i].Temperature = temp;
               }
               else
               {
                  Success = false;  // error
               }
            }
         }

         onMeasurementCompleted(Success);
      }
   }

}

uint8_t tDS1820Sensor::compareAddr(uint8_t* pDeviceAddress1, uint8_t* pDeviceAddress2)
{
   for (int i = 0; i < 8; i++)
   {
      if (pDeviceAddress1[i] != pDeviceAddress2[i])
      {
         return false;
      }
   }
}

uint8_t tDS1820Sensor::findDevID(uint8_t* pDeviceAddress)
{
   if (!isConfigured()) return DS1820_INVALID_ID;

   for (uint8_t i = 0; i < mNumOfDevices; i++)
   {
      if (compareAddr(pDeviceAddress,getCurrentMeasurement()->Dev[i].Addr))
      {
         return i;
      }
   }

   return DS1820_INVALID_ID;
}

#endif // CONFIG_DS1820_SENSOR

#if CONFIG_SENSOR_LOGGER

void tDS1820SensorLogger::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob)
{
    if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
        //TODO
        return;

    tDS1820SensorTypes::tResult_api_v1 *pResult = (tDS1820SensorTypes::tResult_api_v1 *)pDataBlob;
    LOG_PRINT("Measurement completed. SensorID: ");
    LOG(print(SensorID));
    LOG_PRINT(" devs: ");
    LOG(print(pResult->NumOfDevices));
    LOG_PRINT(" Avg: ");
    LOG(print(pResult->Avg));
    uint8_t NumOfItems;
    if (pResult->Avg) {
       NumOfItems = 1;
    }
    else
    {
       NumOfItems = pResult->NumOfDevices;
    }
    for (int i = 0; i < NumOfItems; i++)
    {
       LOG_PRINT(" dev: ");
       LOG(print(i));
       LOG_PRINT(" temp: ");
       LOG(print(((float)(pResult)->Dev[i].Temperature) / 10));
    }
    LOG_PRINTLN("");

}
#endif CONFIG_SENSOR_LOGGER

