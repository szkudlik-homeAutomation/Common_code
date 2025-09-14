/*
 * tSensorHub.cpp
 *
 *  Created on: Oct 28, 2022
 *      Author: mszkudli
 */

#include "../../../global.h"
#if CONFIG_SENSOR_HUB

#include "tSensorHub.h"
#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorFactory.h"
#include "../tMessageReciever.h"

static tSensorHub *tSensorHub::Instance = NULL;

uint8_t tSensorHub::getSensorID(const char * pSensorName)
{
   tSensorCache *pSensorCache = tSensorCache::getByName(pSensorName);
   if (NULL == pSensorCache)
   {
      return SENSOR_ID_NOT_FOUND;
   }

   return pSensorCache->GetSensorID();
}

const char *tSensorHub::getSensorName(uint8_t SensorID, uint8_t deviceId)
{
   tSensorCache *pSensorCache = tSensorCache::getByID(SensorID, deviceId);
   if (NULL == pSensorCache)
   {
      return NULL;
   }
   return (pSensorCache->GetName());
}

uint8_t tSensorHub::getCachedSensorData(uint8_t SensorID, uint8_t deviceId, uint8_t *dataBlobSize, void **pDataBlob)
{
   tSensorCache *pSensorCache = tSensorCache::getByID(SensorID, deviceId);
   if (NULL == pSensorCache)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
   }

   *dataBlobSize = pSensorCache->getDataBlobSize();
   *pDataBlob = pSensorCache->getData();
}


uint8_t tSensorHub::RegisterSensor(uint8_t SensorID, uint8_t deviceId, const __FlashStringHelper *pSensorName)
{
   uint8_t result = STATUS_SUCCESS;
   DEBUG_PRINTLN_3("");
   DEBUG_PRINT_3("Sensor register request. ID: ");
   DEBUG_3(println(SensorID));

   tSensorCache *pSensorCache = tSensorCache::getByID(SensorID, deviceId);
   if (NULL != pSensorCache)
   {
	   DEBUG_PRINTLN_3("-----> Sensor already registgered ");
	   return STATUS_DUPLICATE_ID;
   }
   // add sensor to repository
   pSensorCache = new tSensorCache(SensorID, deviceId);
	if (NULL == pSensorCache) {
		DEBUG_PRINTLN_3("-----> Cannot create sensor cache ");
		return STATUS_OUT_OF_MEMORY;
	}

   result = pSensorCache->setNameProgmem(pSensorName);
   if (result != STATUS_SUCCESS) {
		DEBUG_PRINTLN_3("-----> Cannot set sensor name ");
		delete pSensorCache;
		return result;
	}

#if CONFIG_SENSORS

   // this system has also local sensors, check if the sensor is local

#if CONFIG_REMOTE_SENSORS_TEST
   tSensor *pSensor = NULL;
   if(SensorID == 1)
	   // sensors with IDs > 1 won't be registered in sensorHub locally.
	   // but they may be registetred by handling MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE
	   // don't try to look for IDs > 1 localy when handling such message
	   pSensor = tSensor::getSensor(SensorID);
#else // CONFIG_REMOTE_SENSORS_TEST
     tSensor *pSensor = tSensor::getSensor(SensorID);
#endif // CONFIG_REMOTE_SENSORS_TEST

   result = STATUS_SUCCESS;
   if (pSensor != NULL)
   {
	   pSensorCache->setSensorType(
				pSensor->getSensorType(),
   				pSensor->getSensorApiVersion());

	   // local sensor
	   if(pSensor->isConfigured())
	   {
		   result = pSensorCache->setParams(
					   pSensor->getMeasurementBlobSize(),
					   pSensor->GetMeasurementPeriod());
	   }
	   else
	   {
		   result = STATUS_SENSOR_INCORRECT_STATE;
	   }
   }

#endif //CONFIG_SENSORS
   return result;
}

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tSensorHub::getCachedSensorDataJson(uint8_t SensorID, uint8_t deviceId, Stream *pStream)
{
   tSensorCache *pSensorCache = tSensorCache::getByID(SensorID, deviceId);
   if (NULL == pSensorCache)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
   }

   return pSensorCache->formatJSON(pStream);
}

uint8_t tSensorHub::getCachedSensorsDataJson(Stream *pStream)
{
   tSensorCache *pSensorCache = tSensorCache::getFirst();
   while(NULL != pSensorCache)
   {
      uint8_t result = pSensorCache->formatJSON(pStream);
      pSensorCache = pSensorCache->getNext();
      if (result == STATUS_SUCCESS)
      {
		  if (NULL != pSensorCache)
		  {
			 pStream->print(",");
		  }
		  pStream->print("\r\n");
      }
   }
}

#endif // CONFIG_SENSORS_JSON_OUTPUT

void tSensorHub::onSensorEvent(uint8_t SensorID, uint8_t deviceId, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
{
	uint8_t result;

	DEBUG_PRINTLN_2(">>> ON SENSOR EVENT<<<");

   tSensorCache *pSensorCache = tSensorCache::getByID(SensorID, deviceId);
   if (NULL == pSensorCache)
   {
      return;
   }

   if (! pSensorCache->isWorkingOrReadyState() )
	   return;

#if CONFIG_SENSOR_HUB_GENERATE_EVENTS
   tSensorEvent Event;

   Event.SensorType = pSensorCache->getSensorType();
   Event.SensorID = SensorID;
   Event.ApiVersion = pSensorCache->getSensorApiVersion();
   Event.DeviceId = pSensorCache->getNodeID();

#endif CONFIG_SENSOR_HUB_GENERATE_EVENTS

   if (EventType == EV_TYPE_MEASUREMENT_ERROR)
   {
	  DEBUG_PRINTLN_2(">>> Sensor error reported <<<");
      pSensorCache->setError(tSensorCache::state_sensor_error_reported);
#if CONFIG_SENSOR_HUB_GENERATE_EVENTS
      Event.EventType = EV_TYPE_MEASUREMENT_ERROR;
      Event.dataBlobSize = 0;
      Event.pDataBlob = NULL;
      tMessageReciever::Dispatch(MessageType_SensorEvent, SensorID, &Event);
#endif CONFIG_SENSOR_HUB_GENERATE_EVENTS
      return;
   }
   DEBUG_PRINTLN_2(">>> Data recieved correctly <<<");
   result = pSensorCache->setData(pDataBlob, dataBlobSize);

#if CONFIG_SENSOR_HUB_GENERATE_EVENTS
   if (result == STATUS_SUCCESS)
   {
	   Event.EventType = EventType;
	   Event.dataBlobSize = pSensorCache->getDataBlobSize();
	   Event.pDataBlob = pSensorCache->getData();
	   tMessageReciever::Dispatch(MessageType_SensorEvent, SensorID, &Event);
   }
#endif CONFIG_SENSOR_HUB_GENERATE_EVENTS
}

#endif //CONFIG_SENSOR_HUB
