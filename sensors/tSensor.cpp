/*
 * tSensor.cpp
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tSensor.h"
#include "tDS1820Sensor.h"
#include "tImpulseSensor.h"
#include "tPt100AnalogSensor.h"
#include "tSimpleDigitalInputSensor.h"
#include "tOutputStateSensor.h"
#include "tHeatingCircleStatusSensor.h"
#include "tSystemStatusSensor.h"

tSensor* tSensor::pFirst = NULL;


uint8_t tSensor::Create(uint8_t SensorType, uint8_t sensorID)
{
   if (NULL != getSensor(sensorID))
   {
      return STATUS_DUPLICATE_ID;
   }

   tSensor *pNewSensor = NULL;
   switch (SensorType)
   {
      case SENSOR_TYPE_DS1820:
         pNewSensor = new tDS1820Sensor();
         break;
      case SENSOR_TYPE_IMPULSE:
         pNewSensor = new tImpulseSensor();
         break;
      case SENSOR_TYPE_PT100_ANALOG:
         pNewSensor = new tPt100AnalogSensor();
         break;
      case SENSOR_TYPE_DIGITAL_INPUT:
         pNewSensor = new tSimpleDigitalInputSensor();
         break;
      case SENSOR_TYPE_OUTPUT_STATES:
         pNewSensor = new tOutputStateSensor();
         break;
      case SENSOR_TYPE_HEATING_CIRCLE_STATE:
         pNewSensor = new tHeatingCircleStatusSensor();
         break;
      case SENSOR_TYPE_SYSTEM_STATUS:
          pNewSensor = new tSystemStatusSensor();
          break;
      default:
         return STATUS_UNKNOWN_SENSOR_TYPE;
   }

   if (pNewSensor == NULL)
      return STATUS_SENSOR_CREATE_ERROR;

   pNewSensor->mSensorID = sensorID;
   return STATUS_SUCCESS;
}

tSensor::tSensor(uint8_t SensorType) :
      mCurrentMeasurementBlob(NULL),
      mMeasurementBlobSize(0),
      mSensorType(SensorType),
      mConfigSet(false),
      mMeasurementPeriod(0),
      mCurrMeasurementPeriod(0),
      mpFirstEvent(NULL)
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
   tSensorEvent *pEvent = mpFirstEvent;
   while (NULL != pEvent)
   {
   if (Status)
      {
         pEvent->onEvent(this,EV_TYPE_MEASUREMENT_COMPLETED);
      }
   else
      {
         pEvent->onEvent(this,EV_TYPE_MEASUREMENT_ERROR);
      }

   pEvent = pEvent->mpNext;
   }
}

// static procedures
// executed on cetral node, not on nodes where sensors are actually created
//can't use virtual methods
uint8_t tSensor::TranslateBlobToJSON(uint8_t SensorType, uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
   uint8_t Result = STATUS_UNKNOWN_SENSOR_TYPE;
   switch (SensorType)
   {
      case SENSOR_TYPE_DS1820:
         Result = tDS1820Sensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
      case SENSOR_TYPE_IMPULSE:
         Result = tImpulseSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
      case SENSOR_TYPE_PT100_ANALOG:
         Result = tPt100AnalogSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
      case SENSOR_TYPE_DIGITAL_INPUT:
         Result = tSimpleDigitalInputSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
      case SENSOR_TYPE_OUTPUT_STATES:
         Result = tOutputStateSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
      case SENSOR_TYPE_HEATING_CIRCLE_STATE:
         Result = tHeatingCircleStatusSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
      case SENSOR_TYPE_SYSTEM_STATUS:
          Result = tSystemStatusSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
          break;
   }
   return Result;
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
