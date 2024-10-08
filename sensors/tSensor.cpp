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

#if CONFIG_EEPROM_SENSORS
#include "../../../GlobalDefs/Eeprom.h"
#endif // CONFIG_EEPROM_SENSORS

tSensor* tSensor::pFirst = NULL;
tSensorProcess *tSensorProcess::Instance;

uint8_t tSensor::setConfig(uint16_t measurementPeriod, uint8_t ApiVersion, void *pConfigBlob, uint8_t configBlobSize)
{
	if (mState != SENSOR_CREATED)
	{
		return STATUS_CONFIG_SET_ERROR;
	}

	if (ApiVersion && ApiVersion != getSensorApiVersion())
	{
		DEBUG_PRINTLN_3(" error: api version mismatch");
		return STATUS_CONFIG_SET_ERROR;
	}

	if (NULL != pConfigBlob && NULL != mConfigBlobPtr)
	{
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
	  mSerialEventsMask(0),
	  misMeasurementValid(false),
	  mName(NULL)
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
#if REMOTE_SENSORS_TEST
	if (getSensorID() == 1)
	{
#endif //REMOTE_SENSORS_TEST

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

#if REMOTE_SENSORS_TEST
	}
#endif //REMOTE_SENSORS_TEST

#if CONFIG_SENSOR_GENERATE_SERIAL_EVENTS
  sendSerialMsgSensorEvent(false, EV_TYPE_MEASUREMENT_COMPLETED);	// EV_TYPE_MEASUREMENT_ERROR will be sent if ! isMeasurementValid()
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

#if CONFIG_EEPROM_SENSORS

uint8_t tSensor::SaveToEEprom()
{
    eepromDeleteAllSensors();
    tSensor *pSensor = pFirst;
    uint16_t offset = EEPROM_FIRST_SENSOR;
    uint8_t cnt = 0;
    while (pSensor)
    {
        if (! pSensor->isRunning())
            continue; 

        tEepromSensorEntry Entry;
        Entry.configBlobApiVersion = pSensor->getSensorApiVersion();
        Entry.configBlobSize = pSensor->getConfigBlobSize();
        Entry.eventMask = pSensor->getSensorSerialEventsMask();
        Entry.measurementPeriod = pSensor->GetMeasurementPeriod();
        Entry.sensorID = pSensor->getSensorID();
        Entry.sensorType = pSensor->getSensorType();
        Entry.nameSize = strlen(pSensor->getName());
        EEPROM.put(offset,Entry);
        offset += sizeof(Entry);
        if (offset >= EEPROM.length())
            return STATUS_OUT_OF_MEMORY;

        for (int i = 0; i < Entry.nameSize; i++)
        {
            EEPROM.write(offset++, *(pSensor->getName()+i));
            if (offset >= EEPROM.length())
                return STATUS_OUT_OF_MEMORY;
        }
        for (int i = 0; i < Entry.configBlobSize; i++)
        {
            EEPROM.write(offset++, *(pSensor->getConfigBlob()+i));
            if (offset >= EEPROM.length())
                return STATUS_OUT_OF_MEMORY;
        }

        cnt++;
        pSensor = pSensor->pNext;
    }

    EEPROM.write(EEPROM_NUM_OF_SENSORS, cnt);
    return STATUS_SUCCESS;
}

uint8_t tSensor::RestoreFromEEprom()
{
    uint8_t numOfSesors;
    numOfSesors = EEPROM.read(EEPROM_NUM_OF_SENSORS);
    uint16_t offset = EEPROM_FIRST_SENSOR;
    uint8_t Result;
    while(numOfSesors--)
    {
        tEepromSensorEntry Entry;
        char *name;

        EEPROM.get(offset, Entry);
        offset += sizeof(Entry);
        name = malloc(Entry.nameSize + 1);
        if (NULL == name)
            return STATUS_OUT_OF_MEMORY;
        for (int i = 0; i < Entry.nameSize; i++)
        {
            *(name + i) = EEPROM.read(offset++);
        }
        *(name +  Entry.nameSize) = 0;

        //name won't be copied
        tSensor *pSensor = tSensorFactory::Instance->CreateSensor(Entry.sensorType, Entry.sensorID, name);
        if (NULL == pSensor)
        {
        	// fatal, anyway. No recovery needed
        	return STATUS_SENSOR_CREATE_ERROR;
        }

        /* copy the config */
        if (pSensor->getConfigBlobSize() != Entry.configBlobSize)
        {
        	return STATUS_SENSOR_CREATE_ERROR;
        }

        uint8_t *configBlob = (uint8_t *)pSensor->getConfigBlob();
        for (int i = 0; i < Entry.configBlobSize; i++)
        {
        	configBlob[i] = EEPROM.read(offset++);
        }

        Result = pSensor->setConfig(Entry.measurementPeriod, Entry.configBlobApiVersion);
        if (Result != STATUS_SUCCESS)
        	return Result;

        Result = pSensor->Start();
        if (Result != STATUS_SUCCESS)
        	return Result;
    }

    return STATUS_SUCCESS;
}
#endif // CONFIG_EEPROM_SENSORS


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

#endif //CONFIG_SENSORS
