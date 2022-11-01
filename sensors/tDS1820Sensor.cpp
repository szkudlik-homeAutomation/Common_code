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
   if (dataBlobSize != sizeof(tResult))
   {
         return CREATE_SENSOR_STATUS_OTHER_ERROR;
   }

   tResult *pResult =(tResult *) pDataCache;
   pStream->print(F("\"NumOfDevs\":"));
   pStream->print(pResult->NumOfDevices);
   pStream->print(F(",\"Avg\":"));
   pStream->print(pResult->Avg);
   pStream->print(F(","));
   if (pResult->Avg)
   {
      pStream->print(F("\"Temp\":"));
      pStream->print(pResult->Temp[0]);
      pStream->print(F(","));
   }
   else
   {
      for (uint8_t i = 0; i < pResult->NumOfDevices; i++)
      {
         pStream->print(F("\"Temperature"));
         pStream->print(i);
         pStream->print(F("\":"));
         pStream->print((float)pResult->Temp[i] / 10);
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

   mCurrentMeasurement.Avg = mAvg;
   mCurrentMeasurement.NumOfDevices = mNumOfDevices;
   mCurrentMeasurementBlob = (void*) &mCurrentMeasurement;
   mMeasurementBlobSize = sizeof(tResult); //!!! FIX! real num of devices

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
            mCurrentMeasurement.Temp[0] = 0;
            uint8_t NumOfValidMeasurements = 0;
            for (uint8_t i = 0; i < mNumOfDevices ; i++)
            {
               int16_t temp = round(pDs1820->getTempCByIndex(i) * 10);
               if (temp > -1270)
               {
                  mCurrentMeasurement.Temp[0] += temp;
                  NumOfValidMeasurements++;
               }
               else
               {
                  Success = false;  // error
               }
            }
            mCurrentMeasurement.Temp[0] /= NumOfValidMeasurements;
         }
         else  // mAvg
         {
            Success = true;
            for (uint8_t i = 0; i < mNumOfDevices ; i++)
            {
               int16_t temp = round(pDs1820->getTempCByIndex(i) * 10);
               if (temp > -1270)
               {
                  mCurrentMeasurement.Temp[i] = temp;
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
