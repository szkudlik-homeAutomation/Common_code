/*
 * tSensorDesc.cpp
 *
 *  Created on: Oct 4, 2023
 *      Author: mszkudli
 */
#include "../../../global.h"
#if CONFIG_SENSOR_HUB

#include "tSensorDesc.h"

tSensorDesc * tSensorDesc::pFirst = NULL;


tSensorDesc *tSensorDesc::getByID(uint8_t SensorID)
{
   tSensorDesc *pSensorDesc = pFirst;
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

tSensorDesc *tSensorDesc::getByName(const char * pSensorName)
{
   tSensorDesc *pSensorDesc = pFirst;
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

uint8_t tSensorDesc::setDataBlobSize(uint8_t dataBlobSize)
{
    if (0 == dataBlobSize)
        return STATUS_SUCCESS;

    if (NULL != pDataCache)
        return STATUS_SENSOR_INCORRECT_STATE;

    pDataCache = malloc(dataBlobSize);

    if (NULL == pDataCache)
        return STATUS_OUT_OF_MEMORY;

    mDataBlobSize = dataBlobSize;

    return STATUS_SUCCESS;
}

uint8_t tSensorDesc::formatJSON(Stream *pStream)
{
   uint8_t Result;
   // note that the sensor may be located on a remote machine, use cached data
   pStream->print(F("\""));
   pStream->print(pName);
   pStream->print(F("\":{\"SensorData\":{"));
   if (Status == STATUS_SUCCESS)
   {
	   Result = doFormatJSON(pStream);
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
   pStream->print(F(",\"ID\":"));
   pStream->print(SensorID);
   pStream->print(F(",\"NodeID\":"));
   pStream->print(mNodeID);
   pStream->print(F("}}"));
}
#endif //CONFIG_SENSOR_HUB
