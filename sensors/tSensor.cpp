/*
 * tSensor.cpp
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#include "../../../global.h"

#if CONFIG_SENSORS

#include "tSensor.h"

#include "tSensorHub.h"
#include "../TLE8457_serial/TLE8457_serial_lib.h"
#include "../TLE8457_serial/tOutgoingFrames.h"

tSensor* tSensor::pFirst = NULL;

uint8_t tSensor::setConfig(uint16_t measurementPeriod, uint8_t ApiVersion, void *pConfigBlob, uint8_t configBlobSize)
{
	if (mState != SENSOR_CREATED)
	{
		return STATUS_CONFIG_SET_ERROR;
	}

	if (ApiVersion && (ApiVersion != getSensorApiVersion()))
	{
		DEBUG_PRINTLN_3(" error: api version mismatch");
		return STATUS_CONFIG_SET_ERROR;
	}

	if (NULL != pConfigBlob && NULL != mConfigBlobPtr)
	{
		if ((ApiVersion != getSensorApiVersion()) ||
			 configBlobSize != getConfigBlobSize())
		{
			DEBUG_PRINTLN_3(" error: api version mismatch");
			return STATUS_CONFIG_SET_ERROR;
		}

	    memcpy(mConfigBlobPtr, pConfigBlob, mConfigBlobSize);
	}

	mMeasurementPeriod = measurementPeriod;
    mCurrMeasurementPeriod = measurementPeriod;
    uint8_t status = onSetConfig();
    if (STATUS_SUCCESS == status)
    {
    	mState = SENSOR_PAUSED;
    }

    return status;
}

uint8_t tSensor::setParitalConfig(uint8_t seq, void *data, uint8_t ChunkSize)
{
    if (mState != SENSOR_CREATED)
    {
         return STATUS_CONFIG_SET_ERROR;
    }

    if (seq != mPartialConfigSeq)
    {
        return STATUS_DATA_SEQ_ERROR;
    }

    mPartialConfigSeq++;

    uint8_t configOffset = ChunkSize * seq;
    uint8_t toCopy = mConfigBlobSize - configOffset;
    if (toCopy > ChunkSize) toCopy = ChunkSize;
    if (toCopy == 0)
        return STATUS_DATA_SEQ_ERROR;

    memcpy((uint8_t *)mConfigBlobPtr + configOffset, data, toCopy);

    return STATUS_SUCCESS;
}

/* make the sensor running */
uint8_t tSensor::Start()
{
	uint8_t result;
	if (SENSOR_PAUSED == mState)
	{
		result = onRun();
		if (STATUS_SUCCESS == result)
			mState = SENSOR_RUNNING;
		return result;
	}
	if (SENSOR_RUNNING != mState)
		return STATUS_SENSOR_INCORRECT_STATE;
	return STATUS_SUCCESS;
}

/* pause the sensor */
uint8_t tSensor::Pause()
{
	uint8_t result;
	if (SENSOR_RUNNING == mState)
	{
		result = onPause();
		if (STATUS_SUCCESS == result)
			mState = SENSOR_PAUSED;
		return result;
	}

	return STATUS_SENSOR_INCORRECT_STATE;
}

uint8_t tSensor::Register(char * pSensorName)
{
#if CONFIG_SENSOR_HUB
   tSensorHub::Instance->RegisterLocalSensor(mSensorID, pSensorName, mApiVersion);
#endif //CONFIG_SENSOR_HUB
		   //TODO: send a message to central node in case of remote sensor

   return STATUS_SUCCESS;
}

tSensor::tSensor(uint8_t SensorType, uint8_t sensorID, uint8_t ApiVersion, uint8_t ConfigBlobSize, void *ConfigBlobPtr) :
      mCurrentMeasurementBlob(NULL),
      mMeasurementBlobSize(0),
      mSensorType(SensorType),
      mState(SENSOR_CREATED),
      mMeasurementPeriod(0),
      mCurrMeasurementPeriod(0),
      mSensorID(sensorID),
	  mApiVersion(ApiVersion),
	  mConfigBlobSize(ConfigBlobSize),
	  mConfigBlobPtr(ConfigBlobPtr),
	  mPartialConfigSeq(0)
{
   pNext = pFirst;
   pFirst = this;
   mSerialEventsMask.Byte = 0;
}

tSensor* tSensor::getSensor(uint8_t sensorID)
{
   tSensor *pSensor = pFirst;
   while (pSensor != NULL)
   {
      if (pSensor->getSensorID() == sensorID)
      {
         return pSensor;
      }
      pSensor = pSensor->pNext;
   }

   // not found
   return NULL;
}

void tSensor::onMeasurementCompleted(bool Status)
{
  misMeasurementValid = Status;

#if CONFIG_SENSOR_HUB
  if (Status)
  {
      tSensorHub::Instance->onSensorEvent(getSensorID(), EV_TYPE_MEASUREMENT_COMPLETED, mMeasurementBlobSize, mCurrentMeasurementBlob);
#if CONFIG_TLE8457_COMM_LIB
	   if (mSerialEventsMask.MeasurementCompleted)
	   {
		   sendMsgSensorEventMeasurementCompleted(false);
	   }
#endif

  }
  else
  {
      tSensorHub::Instance->onSensorEvent(getSensorID(), EV_TYPE_MEASUREMENT_ERROR, mMeasurementBlobSize, mCurrentMeasurementBlob);
  }
#endif //CONFIG_SENSOR_HUB
}

#if CONFIG_TLE8457_COMM_LIB
void tSensor::sendMsgSensorEventMeasurementCompleted(bool onDemand)
{
	if (misMeasurementValid)
	{
		uint8_t pos = 0;
		uint8_t seq = 0;
		bool lastSegment = false;
		while (!lastSegment)
		{
			lastSegment = (pos + SENSOR_MEASUREMENT_PAYLOAD_SIZE) >= mMeasurementBlobSize;

			tOutgoingFrames::SendSensorEvent(DEVICE_ID_BROADCAST, getSensorID(), EV_TYPE_MEASUREMENT_COMPLETED, onDemand,
					(uint8_t*)mCurrentMeasurementBlob+pos,
					lastSegment ? mMeasurementBlobSize - pos : SENSOR_MEASUREMENT_PAYLOAD_SIZE,
					seq, lastSegment);
			pos += SENSOR_MEASUREMENT_PAYLOAD_SIZE;
			seq++;
		}
	} else
	{
		tOutgoingFrames::SendSensorEvent(DEVICE_ID_BROADCAST, getSensorID(), EV_TYPE_MEASUREMENT_ERROR, onDemand, NULL, 0, 0, 1);
	}
}
#endif //CONFIG_TLE8457_COMM_LIB

void tSensor::Run()
{
   tSensor *i = pFirst;
   while(i != NULL)
   {
      if (i->isRunning())
      {
         // do the sensor job
         i->doTimeTick();

         // is it a time to trigger measurement?
         if (i->mMeasurementPeriod)
         {
            if (i->mCurrMeasurementPeriod == 0)
            {
               i->mCurrMeasurementPeriod = i->mMeasurementPeriod;
               i->TriggerMeasurement();
            }
            else
            {
               i->mCurrMeasurementPeriod--;
            }
         }
      }
      i = i->pNext;
   }
}

void tSensorProcess::service()
{
   tSensor::Run();
}

void tSensorProcess::setup() {}

#endif //CONFIG_SENSORS
