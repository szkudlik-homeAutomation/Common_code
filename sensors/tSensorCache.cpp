/*
 * tSensorCache.cpp
 *
 *  Created on: Oct 4, 2023
 *      Author: mszkudli
 */
#include "../../../global.h"
#if CONFIG_SENSOR_HUB

#include "tSensorCache.h"
#include "../TLE8457_serial/CommonFramesDefs.h"

tSensorCache * tSensorCache::pFirst = NULL;


tSensorCache *tSensorCache::getByID(uint8_t SensorID)
{
   tSensorCache *pSensorDesc = pFirst;
   while (pSensorDesc != NULL)
   {
      if (pSensorDesc->SensorID == SensorID)
      {
         return pSensorDesc;
      }
      pSensorDesc=pSensorDesc->pNext;
   }

   return pSensorDesc;
}

tSensorCache *tSensorCache::getByName(const char * pSensorName)
{
   tSensorCache *pSensorDesc = pFirst;
   while (pSensorDesc != NULL)
   {
      if (strcmp(pSensorDesc->pName, pSensorName) == 0)
      {
         return pSensorDesc;
      }
      pSensorDesc=pSensorDesc->pNext;
   }

   return pSensorDesc;
}

uint8_t tSensorCache::setDataBlobSize(uint8_t dataBlobSize)
{
	uint8_t MemSize = dataBlobSize;
    if (0 == dataBlobSize)
        return STATUS_SUCCESS;

    if (NULL != pDataCache)
        return STATUS_SENSOR_INCORRECT_STATE;

    mDataBlobSize = dataBlobSize;
    bool doubleSize = false;

    if (mNodeID != 0)   // remote sensor
    {
        // remote sensor, check if extra space is required for incoming data assembly
        if (dataBlobSize > SENSOR_MEASUREMENT_PAYLOAD_SIZE)
        {
        	MemSize *= 2;
            doubleSize = true;
        }
    }

    pDataCache = malloc(MemSize);
    if (NULL == pDataCache)
        return STATUS_OUT_OF_MEMORY;

    if (doubleSize)
    {
        pRemoteDataCache = (uint8_t*)pDataCache + dataBlobSize;
    }
    else
    {
        pRemoteDataCache = NULL;
    }

    return STATUS_SUCCESS;
}

uint8_t tSensorCache::formatJSON(Stream *pStream)
{
   uint8_t Result;
   // note that the sensor may be located on a remote machine, use cached data
   pStream->print(F("\""));
   pStream->print(pName);
   pStream->print(F("\":{\"SensorData\":{"));


   if (Status == STATUS_SUCCESS)
   {
	   if (NULL != mFormatJSON)
	   {
		   Result = mFormatJSON(pStream, this);
	   }
   }
   else if (Status == STATUS_NO_DATA_RECIEVED)
   {
	   Result = STATUS_NO_DATA_RECIEVED;
   }
   else
   {
	   Result = STATUS_SENSOR_ERROR_REPORTED;
	   pStream->print(F("\"SensorSpecificStatus\":"));
	   pStream->print(Status);
   }
   pStream->print(F("},\"GeneralData\":{\"Status\":"));
   pStream->print(Result);
   pStream->print(F(",\"StatusString\":"));
   switch (Result)
   {
   case STATUS_SUCCESS:
   	   pStream->print(F("\"success\""));
   	   break;
   case STATUS_SENSOR_ERROR_REPORTED:
   	   pStream->print(F("\"error\""));
   	   break;
   case STATUS_NO_DATA_RECIEVED:
   	   pStream->print(F("\"no data recieved\""));
   	   break;
   default: pStream->print(F("\"unknown\""));
   }
   pStream->print(F(",\"LastUpdate\":"));
   pStream->print(getTimeSinceUpdate());
   pStream->print(F(",\"ID\":"));
   pStream->print(SensorID);
   pStream->print(F(",\"NodeID\":"));
   pStream->print(mNodeID);
   pStream->print(F("}}"));
}
#endif //CONFIG_SENSOR_HUB
