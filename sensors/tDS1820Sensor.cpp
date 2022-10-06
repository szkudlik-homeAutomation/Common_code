/*
 * tDS1820Sensor.cpp
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#include "../../global.h"
#include "tDS1820Sensor.h"
#include <OneWire.h>
#include "../lib/external/ds1820/DallasTemperature.h"

void tDS1820Sensor::SetSpecificConfig(void *pBlob)
{
   tDS1820SensorConfig *pConfig = (tDS1820SensorConfig*) pBlob;

   OneWire * pOneWire = new OneWire(pConfig->Pin);
   pDs1820 = new DallasTemperature(pOneWire);
   pDs1820->begin();
   pDs1820->setWaitForConversion(false);
   mAvg = pConfig->Avg;

   mCurrentMeasurementBlob = (void*) mCurrentMeasurement;
   mMeasurementBlobSize = mAvg ? sizeof(tDS1820Result)  : sizeof(tDS1820Result) * pDs1820->getDS18Count();

   mConfigSet = true;
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
         uint8_t numOfDev = pDs1820->getDS18Count();
         if (numOfDev > MAX_DS1820_DEVICES_ON_BUS)
            numOfDev = MAX_DS1820_DEVICES_ON_BUS;
         if (mAvg)
         {
            mCurrentMeasurement[0].Temp = 0;
            uint8_t NumOfValidMeasurements = 0;
            for (uint8_t i = 0; i < numOfDev ; i++)
            {
               int16_t temp = round(pDs1820->getTempCByIndex(i) * 10);
               if (temp > -1270)
               {
                  mCurrentMeasurement[0].Temp += temp;
                  NumOfValidMeasurements++;
               }
               else
               {
                  Success = false;  // error
               }
            }
            mCurrentMeasurement[0].Temp /= NumOfValidMeasurements;
         }
         else  // mAvg
         {
            Success = true;
            for (uint8_t i = 0; i < numOfDev ; i++)
            {
               int16_t temp = round(pDs1820->getTempCByIndex(i) * 10);
               if (temp > -1270)
               {
                  mCurrentMeasurement[i].Temp = temp;
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
