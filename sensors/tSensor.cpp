/*
 * tSensor.cpp
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tSensor.h"
#include "tDS1820Sensor.h"

tSensor* tSensor::pFirst = NULL;


uint8_t tSensor::Create(uint8_t SensorType, uint8_t sensorID)
{
   if (NULL != getSensor(sensorID))
   {
      return CREATE_SENSOR_STATUS_DUPLICATE_ID;
   }

   tSensor *pNewSensor = NULL;
   switch (SensorType)
   {
      case SENSOR_TYPE_DS1820:
         pNewSensor = new tDS1820Sensor(sensorID);
         break;
      default:
         return CREATE_SENSOR_STATUS_UNKNOWN_SENSOR;
   }

   if (pNewSensor == NULL)
      return CREATE_SENSOR_STATUS_OTHER_ERROR;

   return CREATE_SENSOR_STATUS_OK;
}

tSensor::tSensor(uint8_t SensorType, uint8_t sensorID) :
      mCurrentMeasurementBlob(NULL),
      mMeasurementBlobSize(0),
      mSensorType(SensorType),
      mConfigSet(false),
      mMeasurementPeriod(0),
      mCurrMeasurementPeriod(0),
      mSensorID(sensorID)
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
   if (NULL != mpEvent)
      {
      if (Status)
         {
            mpEvent->onEvent(this,tSensorEvent::EV_TYPE_MEASUREMENT_COMPLETED);
         }
         else
         {
            mpEvent->onEvent(this,tSensorEvent::EV_TYPE_MEASUREMENT_ERROR);
         }
      }
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
