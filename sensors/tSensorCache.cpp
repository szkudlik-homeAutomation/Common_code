/*
 * tSensorCache.cpp
 *
 *  Created on: Oct 4, 2023
 *      Author: mszkudli
 */
#include "../../../global.h"
#if CONFIG_SENSOR_HUB

#include "tSensorCache.h"
#include "tSensorFactory.h"
#include "../TLE8457_serial/CommonFramesDefs.h"

tSensorCache * tSensorCache::pFirst = NULL;


uint8_t tSensorCache::setParams(uint8_t SensorType, uint8_t ApiVersion, uint8_t nodeID, uint8_t dataBlobSize)
{
	mSensorType = SensorType;
	mSensorApiVersion = ApiVersion;
	mNodeID = nodeID;
	uint8_t result = setDataBlobSize(dataBlobSize);
	if (STATUS_SUCCESS != result)
	{
		mState = state_create_error;
		return STATUS_SENSOR_CREATE_ERROR;
	}
	mFormatJSON = tSensorFactory::Instance->getJSONFormatFunction(mSensorType, mSensorApiVersion);
	mState = state_no_data_recieved;

	return STATUS_SUCCESS;
}

tSensorCache *tSensorCache::getByID(uint8_t SensorID)
{
   tSensorCache *pSensorDesc = pFirst;
   while (pSensorDesc != NULL)
   {
      if (pSensorDesc->mSensorID == SensorID)
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

uint8_t tSensorCache::setData(void *dataSrc, uint8_t dataSize)
{
	if (dataSize != mDataBlobSize)
	{
		setError(state_incorrect_data_size);
		return STATUS_INCORRECT_DATA_SIZE;
	}

	resetTimestamp();
	mState = state_working;
	memcpy(pDataCache, dataSrc, mDataBlobSize);

	return STATUS_SUCCESS;
}

uint8_t tSensorCache::formatJSON(Stream *pStream)
{
   uint8_t SensorStatus = STATUS_SUCCESS;
   // note that the sensor may be located on a remote machine, use cached data
   pStream->print(F("\""));
   pStream->print(pName);
   pStream->print(F("\":{\"SensorData\":{"));

   if (mState == state_working)
   {
	   if (NULL != mFormatJSON)
	   {
		   SensorStatus = mFormatJSON(pStream, this);
	   }
   }

   pStream->print(F("},\"GeneralData\":{\"State\":"));
   pStream->print(mState);
   pStream->print(F(",\"StateString\":"));
   switch (mState)
   {
   case state_not_detected:
   	   pStream->print(F("\"not_detected\""));
   	   break;
   case state_no_data_recieved:
   	   pStream->print(F("\"no_data_recieved\""));
   	   break;
   case state_working:
   	   pStream->print(F("\"working\""));
   	   break;
   case state_timeout:
   	   pStream->print(F("\"timeout\""));
   	   break;
   case state_sensor_error_reported:
   	   pStream->print(F("\"sensor_error_reported\""));
   	   break;
   case state_create_error:
   	   pStream->print(F("\"create_error\""));
   	   break;
   case state_incorrect_data_size:
   	   pStream->print(F("\"incorrect_data_size\""));
   	   break;
   case state_inconsistent_params:
   	   pStream->print(F("\"inconsistent_params\""));
   	   break;

   default: pStream->print(F("\"unknown\""));
   }

   if (SensorStatus != STATUS_SUCCESS)
   {
	   pStream->print(F(",\"SensorStatus\":"));
	   pStream->print(SensorStatus);
   }
   pStream->print(F(",\"LastUpdate\":"));
   pStream->print(getTimeSinceUpdate());
   pStream->print(F(",\"ID\":"));
   pStream->print(mSensorID);
   pStream->print(F(",\"NodeID\":"));
   pStream->print(mNodeID);
   pStream->print(F("}}"));
}
#endif //CONFIG_SENSOR_HUB
