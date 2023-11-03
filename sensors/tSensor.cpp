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

tSensor* tSensor::pFirst = NULL;

uint8_t tSensor::setConfig(uint16_t measurementPeriod, void *pConfigBlob)
{
	if (mState != SENSOR_CREATED)
	{
		return STATUS_CONFIG_SET_ERROR;
	}

	if (NULL != pConfigBlob && NULL != mConfigBlobPtr)
	{
	    memcpy(mConfigBlobPtr, pConfigBlob, mConfigBlobSize);
	}

    mMeasurementPeriod = measurementPeriod;
    mCurrMeasurementPeriod = measurementPeriod;
    uint8_t status = doSetConfig();
    if (STATUS_SUCCESS == status)
    {
    	mState = SENSOR_PAUSED;
    }

    return status;
}

uint8_t tSensor::Register(uint8_t sensorID, char * pSensorName)
{
   if (NULL != getSensor(sensorID))
   {
      DEBUG_PRINT_3("Register duplicate ID: ");
      DEBUG_3(println(mSensorID,DEC));
      return STATUS_DUPLICATE_ID;
   }

   mSensorID = sensorID;
#if CONFIG_CENTRAL_NODE
   tSensorHub::Instance->RegisterLocalSensor(mSensorID, pSensorName);
#endif //CONFIG_CENTRAL_NODE
		   //TODO: send a message to central node in case of remote sensor

   return STATUS_SUCCESS;
}

tSensor::tSensor(uint8_t SensorType, uint8_t ApiVersion, uint8_t ConfigBlobSize, void *ConfigBlobPtr) :
      mCurrentMeasurementBlob(NULL),
      mMeasurementBlobSize(0),
      mSensorType(SensorType),
      mState(SENSOR_CREATED),
      mMeasurementPeriod(0),
      mCurrMeasurementPeriod(0),
      mSensorID(0xFF),
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
  //TODO: remote sensors
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

#endif //CONFIG_SENSORS
