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

uint8_t tSensorCache::setAsDetected()
{
	if (isDetected()) return STATUS_SENSOR_INCORRECT_STATE;
	mState = state_not_configured;
}

uint8_t tSensorCache::setParams(uint8_t SensorType, uint8_t ApiVersion, uint8_t nodeID, uint8_t dataBlobSize, uint16_t measurementPeriod)
{
	if (isConfigured())
		return STATUS_SENSOR_INCORRECT_STATE;

	resetTimestamp();
	mSensorType = SensorType;
	mSensorApiVersion = ApiVersion;
	mNodeID = nodeID;
	mMeasurementPeriod = measurementPeriod;
	uint8_t result = setDataBlobSize(dataBlobSize);
	if (STATUS_SUCCESS != result)
	{
		mState = state_create_error;
		return STATUS_SENSOR_CREATE_ERROR;
	}
#if CONFIG_SENSORS_JSON_OUTPUT
	mFormatJSON = tSensorJsonOutput::Instance->getJSONFormatFunction(mSensorType, mSensorApiVersion);
#endif //CONFIG_SENSORS_JSON_OUTPUT
	mState = state_no_data_recieved;

	return STATUS_SUCCESS;
}

const char SensorPrefix[] PROGMEM = CONFIG_SENSOR_HUB_AUTONAME_PREFIX;
uint8_t tSensorCache::generateName()
{
	uint8_t len = strlen_P(SensorPrefix) + 3;
	mName = malloc(len);
	if (!mName)
		return STATUS_OUT_OF_MEMORY;

	strcpy_P(mName, SensorPrefix);
	mName[len-3] = (mSensorID / 10) + '0';
	mName[len-2] = (mSensorID % 10) + '0';
	mName[len-1] = 0;

	return STATUS_SUCCESS;
}

uint8_t tSensorCache::setNameProgmem(const __FlashStringHelper *pName)
{
	mName = malloc(strlen_P((const char *)pName)+1);
	if (!mName)
		return STATUS_OUT_OF_MEMORY;

	strcpy_P(mName, (const char *)pName);

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
      if (strcmp(pSensorDesc->GetName(), pSensorName) == 0)
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
#if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
    bool doubleSize = false;

    if (! isLocalSensor())
    {
        // remote sensor, check if extra space is required for incoming data assembly
        if (dataBlobSize > SENSOR_MEASUREMENT_PAYLOAD_SIZE)
        {
        	MemSize *= 2;
            doubleSize = true;
        }
    }
#endif CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS

    pDataCache = malloc(MemSize);
    if (NULL == pDataCache)
        return STATUS_OUT_OF_MEMORY;

#if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
    if (doubleSize)
    {
        pRemoteDataCache = (uint8_t*)pDataCache + dataBlobSize;
    }
    else
    {
        pRemoteDataCache = NULL;
    }
#endif CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS

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

void tSensorCache::UpdateTimeout()
{
	if (!isWorkingState())
		return;
	if (getTimeSinceUpdate() > 2*mMeasurementPeriod)
		mState = state_timeout;
	else
		mState = state_working;
}

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tSensorCache::formatJSON(Stream *pStream)
{
   uint8_t SensorStatus = STATUS_SUCCESS;
   if (NULL == GetName())
	   return STATUS_JSON_ENCODE_ERROR;

   // note that the sensor may be located on a remote machine, use cached data
   UpdateTimeout();
   pStream->print(F("\""));
   pStream->print(GetName());
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
   case state_not_seen:
   	   pStream->print(F("\"sensor not detected\""));
   	   break;
   case	state_not_configured:
   	   pStream->print(F("\"sensor not configured\""));
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
   case state_data_transfer_error:
       pStream->print(F("\"data_transfer_error\""));
       break;


   default: pStream->print(F("\"unknown\""));
   }

   if (isConfigured())
   {
	   pStream->print(F(",\"SensorType\":"));
	   pStream->print(mSensorType);

	   pStream->print(F(",\"SensorStatus\":"));
	   pStream->print(SensorStatus);


	   pStream->print(F(",\"Period_100ms\":"));
	   pStream->print(mMeasurementPeriod);

	   pStream->print(F(",\"LastUpdate_100ms\":"));
	   pStream->print(getTimeSinceUpdate());

	   #if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
	   pStream->print(F(",\"NodeID\":"));
	   pStream->print(mNodeID);
	   #endif CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
   }

   pStream->print(F(",\"ID\":"));
   pStream->print(mSensorID);


   pStream->print(F("}}"));

   return STATUS_SUCCESS;
}
#endif CONFIG_SENSORS_JSON_OUTPUT

#if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
uint8_t tSensorCache::addDataSegment(uint8_t SegmentSeq, void *Payload)
{
    if (! isWorkingState())
        return STATUS_SENSOR_INCORRECT_STATE;

    if (SegmentSeq != mSeq)
    {
     // out of order
        resetDataSegment();
        setError(state_data_transfer_error);
        return STATUS_DATA_SEQ_ERROR;
    }

    uint8_t toCopy = SENSOR_MEASUREMENT_PAYLOAD_SIZE;
    uint8_t offset = SegmentSeq * SENSOR_MEASUREMENT_PAYLOAD_SIZE;
    if (toCopy > getDataBlobSize() - offset)
    {
     toCopy = getDataBlobSize() - offset;
    }
    memcpy((uint8_t *)pRemoteDataCache + offset, Payload, toCopy);

    mSeq++;

    return STATUS_SUCCESS;
}
#endif //CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS

#endif //CONFIG_SENSOR_HUB
