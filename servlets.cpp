/*
 * servlets.cpp
 *
 *  Created on: 23 wrz 2022
 *      Author: szkud
 */

#include "../../global.h"
#include "servlets.h"

tDS1820Sensor *tDS1820Servlet::mSensors[MAX_NUM_OF_SENSORS];
uint8_t tDS1820Servlet::mNumOfSensors = 0;


bool tDS1820Servlet::ProcessAndResponse()
{
   // format JSON response
   pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"NumOfSensorsSet\": "));
   pOwner->mEthernetClient.print(mNumOfSensors,DEC);
   pOwner->SendFlashString(PSTR(", \"Sensors \": ["));
   for (uint8_t i = 0; i < mNumOfSensors; i++)
   {
      tDS1820Sensor *pSensor = mSensors[i];
      tDS1820Sensor::tDS1820Result *pDS1820Result =(tDS1820Sensor::tDS1820Result *) pSensor->getMeasurementBlob();
      uint8_t NumOfDevs = pSensor->getMeasurementBlobSize() / sizeof(tDS1820Sensor::tDS1820Result);

      if (i != 0)
         pOwner->SendFlashString(PSTR(","));

      if (pSensor->isMeasurementValid())
      {
         pOwner->SendFlashString(PSTR("{\"Status \": \"OK\""));
      }
      else
      {
         pOwner->SendFlashString(PSTR("{\"Status \": \"ERROR\"}"));
      }

      if (pSensor->isMeasurementValid())
      {
         pOwner->SendFlashString(PSTR(",\"NumOfDevices \": "));
         pOwner->mEthernetClient.print(NumOfDevs,DEC);
         pOwner->SendFlashString(PSTR(", \"Devcies \": ["));

         for (uint8_t dev = 0; dev < NumOfDevs; dev++)
         {
            if (dev != 0)
               pOwner->SendFlashString(PSTR(","));

            pOwner->SendFlashString(PSTR("{\"Temp \": "));
            pOwner->mEthernetClient.print(((float)(pDS1820Result+dev)->Temp) / 10);
            pOwner->SendFlashString(PSTR("}"));
         }
         pOwner->SendFlashString(PSTR("]}"));
      }
   }
   pOwner->SendFlashString(PSTR("]}\r\n"));

   return false;
}

void tDS1820Servlet::registerSensor(tDS1820Sensor *pSensor)
{
   if (mNumOfSensors < MAX_NUM_OF_SENSORS)
   {
      mSensors[mNumOfSensors] = pSensor;
      mNumOfSensors++;
   }
}
