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
#include "../tMessageReciever.h"
#include "../TLE8457_serial/TLE8457_serial_lib.h"
#include "../TLE8457_serial/tOutgoingFrames.h"

tSensor* tSensor::pFirst = NULL;
tSensorProcess *tSensorProcess::Instance;

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
	  mConfigBlobPtr(ConfigBlobPtr)
{
   pNext = pFirst;
   pFirst = this;
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
  }
  else
  {
      tSensorHub::Instance->onSensorEvent(getSensorID(), EV_TYPE_MEASUREMENT_ERROR, mMeasurementBlobSize, mCurrentMeasurementBlob);
  }
#endif //CONFIG_SENSOR_HUB
#if CONFIG_SENSOR_GENERATE_EVENTS
  tSensorEvent Event;
  if (Status)
  {
      Event.EventType = EV_TYPE_MEASUREMENT_COMPLETED;
      Event.SensorType = getSensorType();
      Event.dataBlobSize = mMeasurementBlobSize;
      Event.pDataBlob = mCurrentMeasurementBlob;
  }
  else
  {
      Event.EventType = EV_TYPE_MEASUREMENT_ERROR;
      Event.SensorType = getSensorType();
      Event.dataBlobSize = 0;
      Event.pDataBlob = NULL;
  }

  tMessageReciever::Dispatch(MessageType_SensorEvent, getSensorID(), &Event);
#endif //CONFIG_SENSOR_GENERATE_EVENTS
}

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

void tSensorProcess::onMessage(uint8_t type, uint16_t data, void *pData)
{
#if CONFIG_TLE8457_COMM_LIB
    if (type != MessageType_SerialFrameRecieved)
        return;

    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;
    uint8_t status = STATUS_SUCCESS;

    switch (data)   // messageType
    {
    case MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST:
    {
        HandleMessageGetSensorByIdReqest(pFrame->SenderDevId, (tMessageGetSensorByIdReqest *)pFrame->Data);
        break;
    }
    break;

    }
#endif // CONFIG_TLE8457_COMM_LIB
}

#if CONFIG_TLE8457_COMM_LIB
void tSensorProcess::HandleMessageGetSensorByIdReqest(uint8_t sender, tMessageGetSensorByIdReqest *pFrame)
{
    tSensor *pSensor = tSensor::getSensor(pFrame->SensorID);
    if (NULL != pSensor)
    {
          tMessageGetSensorByIdResponse Response;
          Response.SensorID = pFrame->SensorID;
          Response.MeasurementPeriod = pSensor->GetMeasurementPeriod();
          Response.ApiVersion = pSensor->getSensorApiVersion();
          Response.SensorType = pSensor->getSensorType();
          Response.isConfigured = pSensor->isConfigured();
          Response.isMeasurementValid = pSensor->isMeasurementValid();
          Response.isRunning = pSensor->isRunning();
          Response.EventsMask = pSensor->getSensorSerialEventsMask();
          Response.ConfigBlobSize = pSensor->getConfigBlobSize();
          Response.MeasurementBlobSize = pSensor->getMeasurementBlobSize();
          CommSenderProcess::Instance->Enqueue(sender, MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE, sizeof(Response), &Response);
    }
}

#endif // CONFIG_TLE8457_COMM_LIB
#endif //CONFIG_SENSORS
