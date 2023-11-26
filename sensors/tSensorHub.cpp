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
#include "tSensorDesc.h"
#include "tSensorFactory.h"

static tSensorHub *tSensorHub::Instance = NULL;

uint8_t tSensorHub::getSensorID(const char * pSensorName)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByName(pSensorName);
   if (NULL == pSensorDesc)
   {
      return SENSOR_ID_NOT_FOUND;
   }

   return pSensorDesc->SensorID;
}

const char *tSensorHub::getSensorName(uint8_t SensorID)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return NULL;
   }
   return (pSensorDesc->pName);
}

uint8_t tSensorHub::getCachedSensorData(uint8_t SensorID,  uint8_t *dataBlobSize, void **pDataBlob)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
   }

   *dataBlobSize = pSensorDesc->mDataBlobSize;
   *pDataBlob = pSensorDesc->pDataCache;
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

	   // add sensor to repository
	   tSensorDesc *pSensorDesc = tSensorFactory::Instance->CreateDesc(
			   pSensor->getSensorType(),
			   SensorID,
			   pSensorName,
			   pSensor->getSensorApiVersion(),
			   pSensor->getMeasurementBlobSize()
			   );

	   // send response
	   DEBUG_PRINTLN_3("-----> DONE, SUCCESS");
	   return STATUS_SUCCESS;
}

void tSensorHub::getSensorInfoRequest(uint8_t SensorID)
{
   //TODO: async communication with external nodes
   tSensor *pSensor = tSensor::getSensor(SensorID);
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if ((NULL == pSensor) || (NULL == pSensorDesc))
   {
      getSensorInfoResponse(STATUS_UNKNOWN_SENSOR_ID, 0, 0, 0, 0, 0, NULL);
      return;
   }

   getSensorInfoResponse(
         STATUS_SUCCESS,
         pSensorDesc->SensorID,
         0, // deviceID TBD
         pSensorDesc->sensorType,
         pSensor->isRunning(),
         pSensor->GetMeasurementPeriod(),
         pSensorDesc->pName);
}

uint8_t tSensorHub::subscribeToEvents(uint8_t SensorID, tSensorHubEvent *pSensorEvent)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
   }

   pSensorEvent->Connect(&pSensorDesc->pFirstEventHander);
   return STATUS_SUCCESS;
}

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tSensorHub::getCachedSensorDataJson(uint8_t SensorID, Stream *pStream)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
   }

   return pSensorDesc->formatJSON(pStream);
}

uint8_t tSensorHub::getCachedSensorsDataJson(Stream *pStream)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getFirst();
   while(NULL != pSensorDesc)
   {
      pSensorDesc->formatJSON(pStream);
      pSensorDesc = pSensorDesc->getNext();
      if (NULL!=pSensorDesc)
      {
         pStream->print(",");
      }
      pStream->print("\r\n");
   }
}

#endif // CONFIG_SENSORS_JSON_OUTPUT

void tSensorHub::callAllCallbacks(tSensorDesc *pSensorDesc, uint8_t EventType)
{
   // callbacks
   tSensorHubEvent *pEventCallback = pSensorDesc->pFirstEventHander;
   while (pEventCallback)
   {
      if (EV_TYPE_MEASUREMENT_ERROR == EventType)
         pEventCallback->onEvent(pSensorDesc->SensorID, EventType, pSensorDesc->mDataBlobSize, pSensorDesc->pDataCache);
      else
         pEventCallback->onEvent(pSensorDesc->SensorID, EventType, 0, NULL);

      pEventCallback = pEventCallback->pNext;
   }
}

void tSensorHub::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return;
   }

   if (EventType == EV_TYPE_MEASUREMENT_ERROR)
   {
      pSensorDesc->Status = STATUS_SENSOR_ERROR_REPORTED;
      // callbacks
      callAllCallbacks(pSensorDesc,EV_TYPE_MEASUREMENT_ERROR);
      return;
   }

   if (dataBlobSize != pSensorDesc->mDataBlobSize)
   {
       pSensorDesc->Status = STATUS_INCORRECT_DATA_SIZE;
       return;
   }

   pSensorDesc->Status = STATUS_SUCCESS;
   if (0 != dataBlobSize)
       memcpy(pSensorDesc->pDataCache,pDataBlob,dataBlobSize);

   // callbacks
   callAllCallbacks(pSensorDesc,EventType);
}



#endif //CONFIG_SENSOR_HUB
