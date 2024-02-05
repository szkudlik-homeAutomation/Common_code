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

   return pSensorCache->SensorID;
}

const char *tSensorHub::getSensorName(uint8_t SensorID)
{
   tSensorCache *pSensorCache = tSensorCache::getByID(SensorID);
   if (NULL == pSensorCache)
   {
      return NULL;
   }
   return (pSensorCache->pName);
}

uint8_t tSensorHub::getCachedSensorData(uint8_t SensorID,  uint8_t *dataBlobSize, void **pDataBlob)
{
   tSensorCache *pSensorCache = tSensorCache::getByID(SensorID);
   if (NULL == pSensorCache)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
   }

   *dataBlobSize = pSensorCache->getDataBlobSize();
   *pDataBlob = pSensorCache->pDataCache;
}


uint8_t tSensorHub::RegisterLocalSensor(uint8_t SensorID, char * pSensorName)
{
	   DEBUG_PRINTLN_3("");
	   DEBUG_PRINT_3("==>Sensor register request. ID: ");
	   DEBUG_3(print(SensorID));
	   DEBUG_PRINT_3(" name: ");
	   DEBUG_3(println(pSensorName));

	   tSensor *pSensor = tSensor::getSensor(SensorID);
	   if (pSensor == NULL)
	   {
	      DEBUG_PRINTLN_3("-----> tSensor::getSensor error ");
	      return STATUS_UNKNOWN_SENSOR_ID;
	   }

       tSensorCache *pSensorCache = tSensorCache::getByID(SensorID);
       if (NULL != pSensorCache)
       {
           DEBUG_PRINTLN_3("-----> Sensor already registgered ");
           return STATUS_DUPLICATE_ID;
       }

	   // add sensor to repository
	   pSensorCache = new tSensorCache;
	   pSensorCache->sensorType = pSensor->getSensorType();
	   pSensorCache->SensorID = SensorID;
	   pSensorCache->pName = pSensorName;
	   pSensorCache->sensorApiVersion = pSensor->getSensorApiVersion();
	   pSensorCache->mNodeID = 0; /* local sensor */
	   pSensorCache->Status = STATUS_NO_DATA_RECIEVED;
	   pSensorCache->mFormatJSON = tSensorFactory::Instance->getJSONFormatFunction(pSensorCache->sensorType, pSensorCache->sensorApiVersion);
	   uint8_t result = pSensorCache->setDataBlobSize(pSensor->getMeasurementBlobSize());
	   if (STATUS_SUCCESS != result)
	   {
		   return result;    // no recovery, no need
	   }

	   DEBUG_PRINTLN_3("-----> DONE, SUCCESS");
	   return STATUS_SUCCESS;
}


#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tSensorHub::getCachedSensorDataJson(uint8_t SensorID, Stream *pStream)
{
   tSensorCache *pSensorCache = tSensorCache::getByID(SensorID);
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
      pSensorCache->formatJSON(pStream);
      pSensorCache = pSensorCache->getNext();
      if (NULL!=pSensorCache)
      {
         pStream->print(",");
      }
      pStream->print("\r\n");
   }
}

#endif // CONFIG_SENSORS_JSON_OUTPUT

void tSensorHub::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
{
#if CONFIG_SENSOR_HUB_GENERATE_EVENTS
   tSensorEvent Event;
#endif CONFIG_SENSOR_HUB_GENERATE_EVENTS

   tSensorCache *pSensorCache = tSensorCache::getByID(SensorID);
   if (NULL == pSensorCache)
   {
      return;
   }

   pSensorCache->resetTimestamp();

   if (EventType == EV_TYPE_MEASUREMENT_ERROR)
   {
      pSensorCache->Status = STATUS_SENSOR_ERROR_REPORTED;
#if CONFIG_SENSOR_HUB_GENERATE_EVENTS
      Event.EventType = EV_TYPE_MEASUREMENT_ERROR;
      Event.SensorType = pSensorCache->sensorType;
      Event.dataBlobSize = 0;
      Event.pDataBlob = NULL;
      tMessageReciever::Dispatch(MessageType_SensorEvent, SensorID, &Event);
#endif CONFIG_SENSOR_HUB_GENERATE_EVENTS
      return;
   }

   if (dataBlobSize != pSensorCache->getDataBlobSize())
   {
       pSensorCache->Status = STATUS_INCORRECT_DATA_SIZE;
       return;
   }

   pSensorCache->Status = STATUS_SUCCESS;
   if (0 != dataBlobSize)
       memcpy(pSensorCache->pDataCache,pDataBlob,dataBlobSize);

#if CONFIG_SENSOR_HUB_GENERATE_EVENTS
   Event.EventType = EventType;
   Event.dataBlobSize = pSensorCache->mDataBlobSize;
   Event.pDataBlob = pSensorCache->pDataCache;
   tMessageReciever::Dispatch(MessageType_SensorEvent, SensorID, &Event);
#endif CONFIG_SENSOR_HUB_GENERATE_EVENTS
}

#endif //CONFIG_SENSOR_HUB
