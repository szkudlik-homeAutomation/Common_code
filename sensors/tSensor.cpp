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
#include "tSensorFactory.h"
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
	  mConfigBlobPtr(ConfigBlobPtr),
	  mPartialConfigSeq(0),
	  mSerialEventsMask(0)
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
  if (getSensorID() == 1)	//TEST
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
#if CONFIG_SENSOR_GENERATE_SERIAL_EVENTS
  sendSerialMsgSensorEvent(false, EV_TYPE_MEASUREMENT_COMPLETED);	// EV_TYPE_MEASUREMENT_ERROR will be sent if ! misMeasurementValid
#endif // CONFIG_SENSOR_GENERATE_SERIAL_EVENTS
}

#if CONFIG_SENSOR_GENERATE_SERIAL_EVENTS
void tSensor::sendSerialMsgSensorEvent(bool onDemand, uint8_t SensorEventType)
{
	if (misMeasurementValid)
	{
		uint8_t pos = 0;
		uint8_t seq = 0;
		bool lastSegment = false;
		if (onDemand || (mSerialEventsMask & (1 << SensorEventType)))
		{
			while (!lastSegment)
			{
				lastSegment = (pos + SENSOR_MEASUREMENT_PAYLOAD_SIZE) >= mMeasurementBlobSize;

				tOutgoingFrames::SendSensorEvent(DEVICE_ID_BROADCAST, getSensorID(), SensorEventType, onDemand,
						(uint8_t*)mCurrentMeasurementBlob+pos,
						lastSegment ? mMeasurementBlobSize - pos : SENSOR_MEASUREMENT_PAYLOAD_SIZE,
						seq, lastSegment);
				pos += SENSOR_MEASUREMENT_PAYLOAD_SIZE;
				seq++;
			}
		}
	}
	else
	{
		if (onDemand || (mSerialEventsMask & (1 << EV_TYPE_MEASUREMENT_ERROR)))
			tOutgoingFrames::SendSensorEvent(DEVICE_ID_BROADCAST, getSensorID(), EV_TYPE_MEASUREMENT_ERROR, onDemand, NULL, 0, 0, 1);
	}
}
#endif //CONFIG_SENSOR_GENERATE_SERIAL_EVENTS

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
#if CONFIG_SENSORS_OVER_SERIAL_COMM
    if (type != MessageType_SerialFrameRecieved)
        return;

    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;
    uint8_t status = STATUS_SUCCESS;

    switch (data)   // messageType
    {
    case MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST:
        HandleMessageGetSensorByIdReqest(pFrame->SenderDevId, (tMessageGetSensorByIdReqest *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_CREATE:
        HandleMsgSensorCreate(pFrame->SenderDevId, (tMessageSensorCreate *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_CONFIGURE:
        HandleMsgSensorConfigure(pFrame->SenderDevId, (tMessageSensorConfigure *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_START:
        HandleMsgSensorStart(pFrame->SenderDevId, (tMessageSensorStart *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_STOP:
        HandleMsgSensorStop(pFrame->SenderDevId, (tMessageSensorStop *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST:
        HandleMsgGetSensorMeasurementReqest(SenderDevId,(tMessageGetSensorMeasurementReqest*)(pFrame->Data));
        break;
    }
#endif // CONFIG_SENSORS_OVER_SERIAL_COMM
}
#if CONFIG_SENSORS_OVER_SERIAL_COMM
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

void tSensorProcess::HandleMsgSensorCreate(uint8_t sender, tMessageSensorCreate *pFrame)
{
    DEBUG_PRINT_3("Creating type: ");
    DEBUG_3(print(pFrame->SensorType, DEC));
    DEBUG_PRINT_3(" with ID: ");
    DEBUG_3(println(pFrame->SensorID, DEC));

    tSensor *pSensor = tSensorFactory::Instance->CreateSensor(pFrame->SensorType, pFrame->SensorID);

    if (pSensor)
        tOutgoingFrames::SendMsgStatus(sender, 0);
    else
        tOutgoingFrames::SendMsgStatus(sender, STATUS_GENERAL_FAILURE);
}

void tSensorProcess::HandleMsgSensorConfigure(uint8_t SenderID, tMessageSensorConfigure *Message)
{
    uint8_t result;
    tSensor *pSensor = tSensor::getSensor(Message->Header.SensorID);
    if (!pSensor)
        return;

    DEBUG_PRINT_3("Setting config for sensor ID: ");
    DEBUG_3(println(Message->Header.SensorID, DEC));

    if (Message->Header.LastSegment == 1)
    {
        result = pSensor->setConfig(Message->Data.MeasurementPeriod);
        tOutgoingFrames::SendMsgStatus(SenderID, result);
    }
    else
    {
        result = pSensor->setParitalConfig(Message->Header.SegmentSeq, Message->Payload, SENSOR_CONFIG_PAYLOAD_SIZE);
        if (result != STATUS_SUCCESS)
        {
            tOutgoingFrames::SendMsgStatus(SenderID, result);
        }
    }
}


void tSensorProcess::HandleMsgSensorStart(uint8_t SenderID, tMessageSensorStart *Message)
{
    tSensor *pSensor = tSensor::getSensor(Message->SensorID);
    if (!pSensor)
        return;

    DEBUG_PRINT_3("Starting sensor ID: ");
    DEBUG_3(print(Message->SensorID, DEC));
    DEBUG_PRINT_3(" with event mask: ");
    DEBUG_3(println(Message->SensorEventMask, BIN));

    uint8_t result = pSensor->Start();
    if (STATUS_SUCCESS == result)
        pSensor->setSensorSerialEventsMask(Message->SensorEventMask);

    DEBUG_PRINT_3("   status: ");
    DEBUG_3(println(result, DEC));

    tOutgoingFrames::SendMsgStatus(SenderID, result);
}

void tSensorProcess::HandleMsgSensorStop(uint8_t SenderID, tMessageSensorStop *Message)
{
    tSensor *pSensor = tSensor::getSensor(Message->SensorID);
    if (!pSensor)
        return;

    DEBUG_PRINT_3("Stopping sensor ID: ");
    DEBUG_3(println(Message->SensorID, DEC));

    uint8_t result = pSensor->Pause();

    DEBUG_PRINT_3("   status: ");
    DEBUG_3(println(result, DEC));

    tOutgoingFrames::SendMsgStatus(SenderID, result);
}

void tSensorProcess::HandleMsgGetSensorMeasurementReqest(uint8_t SenderID, tMessageGetSensorMeasurementReqest *Message)
{
    /* get data blob from sensor and send it in one or more frames */
    tSensor *pSensor = tSensor::getSensor(Message->SensorID);
    if (NULL == pSensor)
        return;

    pSensor->sendSerialMsgSensorEvent(true, EV_TYPE_MEASUREMENT_COMPLETED);
}

#endif // CONFIG_TLE8457_COMM_LIB

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

#endif //CONFIG_SENSORS_OVER_SERIAL_COMM
