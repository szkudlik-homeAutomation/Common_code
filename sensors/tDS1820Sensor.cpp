/*
 * tDS1820Sensor.cpp
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tDS1820Sensor.h"
#include <OneWire.h>
#include "../../lib/external/ds1820/DallasTemperature.h"

uint8_t tDS1820Sensor::TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
   tResult *pResult =(tResult *) pDataCache;
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
         printAddress((uint8_t*)&pResult->Dev[i].Addr,pStream);
         pStream->print(F("\":"));
         pStream->print((float)pResult->Dev[i].Temperature / 10);
         pStream->print(F(","));
      }
   }

   return CREATE_SENSOR_STATUS_OK;
}

uint8_t tDS1820Sensor::SetSpecificConfig(void *pBlob)
{
   tConfig *pConfig = (tConfig*) pBlob;

   OneWire * pOneWire = new OneWire(pConfig->Pin);
   pDs1820 = new DallasTemperature(pOneWire);
   pDs1820->begin();
   pDs1820->setWaitForConversion(false);
   mAvg = pConfig->Avg;
   mNumOfDevices = pConfig->NumOfDevices;
   if (mNumOfDevices > MAX_DS1820_DEVICES_ON_BUS)
      mNumOfDevices  = MAX_DS1820_DEVICES_ON_BUS;

   // calculate size of the result
   mMeasurementBlobSize = sizeof(tResult) + (sizeof(tDs1820Data) * mNumOfDevices);
   mCurrentMeasurementBlob = malloc(mMeasurementBlobSize);

   getCurrentMeasurement()->Avg = mAvg;
   getCurrentMeasurement()->NumOfDevices = mNumOfDevices;

   if (! mAvg)
   {
      // set device IDs
      for (uint8_t i = 0; i < mNumOfDevices; i++)
      {
         bool DevExist = pDs1820->getAddress(getCurrentMeasurement()->Dev[i].Addr,i);
         if (! DevExist)
         {
            return CREATE_SENSOR_STATUS_CONFIG_SET_ERROR;
         }
      }
   }
   mConfigSet = true;
   return CREATE_SENSOR_STATUS_OK;
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
         else if (mAvg)
         {
            getCurrentMeasurement()->Dev[0].Temperature = 0;
            uint8_t NumOfValidMeasurements = 0;
            for (uint8_t i = 0; i < mNumOfDevices ; i++)
            {
               int16_t temp = round(pDs1820->getTempCByIndex(i) * 10);
               if (temp > -1270)
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
         else  // mAvg
         {
            Success = true;
            for (uint8_t i = 0; i < mNumOfDevices ; i++)
            {
               int16_t temp = round(pDs1820->getTempCByIndex(i) * 10);
               if (temp > -1270)
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
   if (! mConfigSet) return DS1820_INVALID_ID;

   for (uint8_t i = 0; i < mNumOfDevices; i++)
   {
      if (compareAddr(pDeviceAddress,getCurrentMeasurement()->Dev[i].Addr))
      {
         return i;
      }
   }

   return DS1820_INVALID_ID;
}
