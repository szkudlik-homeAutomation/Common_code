/*
 * tSensor.cpp
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#include "../../../global.h"

#if CONFIG_SENSORS

#include "tSensor.h"

#if CONFIG_DS1820_SENSOR
#include "tDS1820Sensor.h"
#endif

#if CONFIG_IMPULSE_SENSOR
#include "tImpulseSensor.h"
#endif

#if CONFIG_PT100_ANALOG_SENSOR
#include "tPt100AnalogSensor.h"
#endif

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
#include "tSimpleDigitalInputSensor.h"
#endif

#if CONFIG_OUTPUT_STATE_SENSOR
#include "tOutputStateSensor.h"
#endif

#if CONFIG_SYSTEM_STATUS_SENSOR
#include "tSystemStatusSensor.h"
#endif

#include "tSensorHub.h"

tSensor* tSensor::pFirst = NULL;


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
   SensorHub.RegisterLocalSensor(mSensorID, pSensorName);
#endif //CONFIG_CENTRAL_NODE
		   //TODO: send a message to central node in case of remote sensor

   return STATUS_SUCCESS;
}

tSensor::tSensor(uint8_t SensorType) :
      mCurrentMeasurementBlob(NULL),
      mMeasurementBlobSize(0),
      mSensorType(SensorType),
      mConfigSet(false),
      mMeasurementPeriod(0),
      mCurrMeasurementPeriod(0),
      mSensorID(0xFF)
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
	  SensorHub.onSensorEvent(getSensorID(), EV_TYPE_MEASUREMENT_COMPLETED, mMeasurementBlobSize, mCurrentMeasurementBlob);
  }
  else
  {
	  SensorHub.onSensorEvent(getSensorID(), EV_TYPE_MEASUREMENT_ERROR, mMeasurementBlobSize, mCurrentMeasurementBlob);
  }
#endif //CONFIG_SENSOR_HUB
  //TODO: remote sensors
}

// static procedures
// executed on cetral node, not on nodes where sensors are actually created
// can't use virtual methods - all static
#if CONFIG_SENSORS_JSON_OUTPUT

__attribute__((weak)) uint8_t appTranslateBlobToJSON(uint8_t SensorType, uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
	return STATUS_UNKNOWN_SENSOR_TYPE;
}

uint8_t TranslateBlobToJSON(uint8_t SensorType, uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
   uint8_t Result = STATUS_UNKNOWN_SENSOR_TYPE;
   switch (SensorType)
   {
#if CONFIG_DS1820_SENSOR
      case SENSOR_TYPE_DS1820:
         Result = tDS1820Sensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
#endif // CONFIG_DS1820_SENSOR

#if CONFIG_IMPULSE_SENSOR
      case SENSOR_TYPE_IMPULSE:
         Result = tImpulseSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
#endif //CONFIG_IMPULSE_SENSOR

#if CONFIG_PT100_ANALOG_SENSOR
         case SENSOR_TYPE_PT100_ANALOG:
         Result = tPt100AnalogSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
#endif

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
      case SENSOR_TYPE_DIGITAL_INPUT:
         Result = tSimpleDigitalInputSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
#endif

#if CONFIG_OUTPUT_STATE_SENSOR
     case SENSOR_TYPE_OUTPUT_STATES:
         Result = tOutputStateSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
         break;
#endif

#if CONFIG_SYSTEM_STATUS_SENSOR
      case SENSOR_TYPE_SYSTEM_STATUS:
          Result = tSystemStatusSensor::TranslateBlobToJSON(dataBlobSize,pDataCache,pStream);
          break;
#endif //CONFIG_SYSTEM_STATUS_SENSOR

      default:
    	  Result = appTranslateBlobToJSON(SensorType,dataBlobSize,pDataCache,pStream);
          break;
   }
   return Result;
}
#endif // CONFIG_SENSORS_JSON_OUTPUT

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
