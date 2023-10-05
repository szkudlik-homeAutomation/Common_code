/*
 * tDS1820Sensor.cpp
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#include "../../../global.h"

#if CONFIG_DS1820_SENSOR

#include "tDS1820Sensor.h"
#include "../../lib/OneWire/OneWire.h"
#include "../../lib/ds1820/DallasTemperature.h"

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tDs1820SensorDesc::doFormatJSON(Stream *pStream)
{
   if (dataBlobSize < sizeof(tDS1820Sensor::tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tDS1820Sensor::tResult *pResult =(tDS1820Sensor::tResult *) pDataCache;
   uint8_t MeasurementBlobSize = sizeof(tDS1820Sensor::tResult) +
		   (sizeof(tDS1820Sensor::tDs1820Data) * pResult->NumOfDevices);
   if (dataBlobSize != MeasurementBlobSize)
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   pStream->print(F("\"NumOfDevs\":"));
   pStream->print(pResult->NumOfDevices);
   pStream->print(F(",\"Avg\":"));
   pStream->print(pResult->Avg);
   pStream->print(F(","));
   if (pResult->Avg)
   {
      pStream->print(F("\"AvgTemperature\":"));
      pStream->print((float)pResult->Dev[0].Temperature / 10);
      pStream->print(F(","));
   }
   else
   {
      for (uint8_t i = 0; i < pResult->NumOfDevices; i++)
      {
         pStream->print(F("\"Temperature_"));
         tDS1820Sensor::printAddress((uint8_t*)&pResult->Dev[i].Addr,pStream);
         pStream->print(F("\":"));
         pStream->print((float)pResult->Dev[i].Temperature / 10);
         pStream->print(F(","));
      }
   }

   return STATUS_SUCCESS;
}
#endif //CONFIG_SENSORS_JSON_OUTPUT

uint8_t tDS1820Sensor::doSetConfig()
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
#ifdef DEBUG_3_ENABLE
         else
         {
            DEBUG_PRINT_3("             FOUND, serial: ");
            // zonk, response_handeler is not a stream... so serial only
            printAddress((uint8_t*)getCurrentMeasurement()->Dev[i].Addr, &DEBUG_SERIAL);
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
void tDS1820Sensor::printAddress(uint8_t* pDeviceAddress, Stream *pStream)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (pDeviceAddress[i] < 16) pStream->print("0");
    pStream->print(pDeviceAddress[i], HEX);
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
