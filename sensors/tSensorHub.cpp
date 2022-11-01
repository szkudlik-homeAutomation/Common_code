/*
 * tSensorHub.cpp
 *
 *  Created on: Oct 28, 2022
 *      Author: mszkudli
 */

#include "tSensorHub.h"
#include "tSensor.h"

tSensorHub::tSensorDesc * tSensorHub::tSensorDesc::pFirst = NULL;


tSensorHub::tSensorDesc *tSensorHub::tSensorDesc::getByID(uint8_t SensorID)
{
   tSensorDesc *pSensorDesc = pFirst;
   while (pSensorDesc != NULL)
   {
      if (pSensorDesc->SensorID == SensorID)
      {
         return pSensorDesc;
      }
      pSensorDesc=pSensorDesc->pNext;
   }

   return pSensorDesc;
}

tSensorHub::tSensorDesc *tSensorHub::tSensorDesc::getByName(const char * pSensorName)
{
   tSensorDesc *pSensorDesc = pFirst;
   while (pSensorDesc != NULL)
   {
      if (strcmp(pSensorDesc->pName, pSensorName) == 0)
      {
         return pSensorDesc;
      }
      pSensorDesc=pSensorDesc->pNext;
   }

   return pSensorDesc;
}

uint8_t tSensorHub::getSensorID(const char * pSensorName)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByName(pSensorName);
   if (NULL == pSensorDesc)
   {
      return SENSOR_NOT_FOUND;
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
      return SENSOR_NOT_FOUND;
   }

   *dataBlobSize = pSensorDesc->dataBlobSize;
   *pDataBlob = pSensorDesc->pDataCache;
}

void tSensorHub::CreateSensorRequest(uint8_t TargetNode, uint8_t SensorType, uint8_t SensorID, char * pSensorName, void* pConfigBlob, uint8_t MeasurementPeriod)
{
   //TODO: async sensor creation on external nodes

   uint8_t Status;
   (void) TargetNode; // not yet implemented

   // create a sensor on a local node

   Status = tSensor::Create(SensorType, SensorID);
   if (Status != CREATE_SENSOR_STATUS_OK)
   {
      CreateSensorResponse(SensorID, Status);
      return;
   }

   tSensor *pSensor = tSensor::getSensor(SensorID);
   if (pSensor == NULL)
   {
      CreateSensorResponse(SensorID, CREATE_SENSOR_STATUS_OTHER_ERROR);
      return;
   }

   if (NULL != pConfigBlob)
   {
      Status = pSensor->SetSpecificConfig(pConfigBlob);
      if (Status != CREATE_SENSOR_STATUS_OK)
      {
         CreateSensorResponse(SensorID, Status);
         return;
      }
   }
   pSensor->SetMeasurementPeriod(MeasurementPeriod);

   // add sensor to repository
   tSensorDesc *pSensorDesc = new tSensorDesc;
   pSensorDesc->SensorID = SensorID;
   pSensorDesc->pDataCache = NULL;
   pSensorDesc->pFirstEventHander = NULL;
   pSensorDesc->dataBlobSize = 0;
   pSensorDesc->pName = pSensorName;
   pSensorDesc->sensorType = SensorType;
   pSensorDesc->Status = tSensorDesc::STATUS_NO_DATA_RECIEVED;

   pSensor->SetEventCalback(this);
   // send response
   CreateSensorResponse(SensorID, CREATE_SENSOR_STATUS_OK);
}

void tSensorHub::getSensorInfoRequest(uint8_t SensorID)
{
   //TODO: async communication with external nodes
   tSensor *pSensor = tSensor::getSensor(SensorID);
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if ((NULL == pSensor) || (NULL == pSensorDesc))
   {
      getSensorInfoResponse(SENSOR_NOT_FOUND, 0, 0, 0, 0, 0, NULL);
      return;
   }

   getSensorInfoResponse(
         CREATE_SENSOR_STATUS_OK,
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
      return SENSOR_NOT_FOUND;
   }

   pSensorEvent->Connect(&pSensorDesc->pFirstEventHander);
   return CREATE_SENSOR_STATUS_OK;
}

uint8_t tSensorHub::getCachedSensorDataJson(uint8_t SensorID, Stream *pStream)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return SENSOR_NOT_FOUND;
   }

   return formatJSON(pSensorDesc,pStream);
}

uint8_t tSensorHub::getCachedSensorsDataJson(Stream *pStream)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getFirst();
   while(NULL != pSensorDesc)
   {
      formatJSON(pSensorDesc,pStream);
      pSensorDesc = pSensorDesc->getNext();
      if (NULL!=pSensorDesc)
      {
         pStream->print(",");
      }
   }
}

uint8_t tSensorHub::formatJSON(tSensorDesc *pSensorDesc, Stream *pStream)
{
   uint8_t Result;
   // note that the sensor may be located on a remote machine, use cached data
   pStream->print(F("{\""));
   pStream->print(pSensorDesc->pName);
   pStream->print(F("\":{"));
   if (pSensorDesc->Status == tSensorDesc::STATUS_OPERATIONAL)
   {
      Result = tSensor::TranslateBlobToJSON(pSensorDesc->sensorType, pSensorDesc->dataBlobSize, pSensorDesc->pDataCache, pStream);
   }
   else
   {
      Result = pSensorDesc->Status; //!!! todo - statuses
   }
   pStream->print(F("\"Status\":"));
   pStream->print(Result);
   pStream->print(F(",\"ID\":"));
   pStream->print(pSensorDesc->SensorID);
   pStream->print(F("}}"));
}

void tSensorHub::callAllCallbacks(tSensorDesc *pSensorDesc, tSensorEventType EventType)
{
   // callbacks
   tSensorHubEvent *pEventCallback = pSensorDesc->pFirstEventHander;
   while (pEventCallback)
   {
      if (EV_TYPE_MEASUREMENT_ERROR != EventType)
         pEventCallback->onEvent(pSensorDesc->SensorID, EventType, pSensorDesc->dataBlobSize, pSensorDesc->pDataCache);
      else
         pEventCallback->onEvent(pSensorDesc->SensorID, EventType, 0, NULL);

      pEventCallback = pEventCallback->pNext;
   }
}

void tSensorHub::onSensorEvent(uint8_t SensorID, tSensorEventType EventType, uint8_t dataBlobSize, void *pDataBlob)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return;
   }

   if (EventType == EV_TYPE_MEASUREMENT_ERROR)
   {
      pSensorDesc->Status = tSensorDesc::STATUS_ERROR_REPORTED;
      // callbacks
      callAllCallbacks(pSensorDesc,EV_TYPE_MEASUREMENT_ERROR);
      return;
   }

   if ((pSensorDesc->dataBlobSize == 0) && (dataBlobSize > 0))
   {
      pSensorDesc->dataBlobSize = dataBlobSize;
      pSensorDesc->pDataCache = malloc(dataBlobSize);
   }

   if (pSensorDesc->dataBlobSize != dataBlobSize)
   {
      pSensorDesc->Status = tSensorDesc::STATUS_ERROR_INCORRECT_DATA_SIZE;
      callAllCallbacks(pSensorDesc,EV_TYPE_MEASUREMENT_ERROR);
      return;
   }

   pSensorDesc->Status = tSensorDesc::STATUS_OPERATIONAL;
   memcpy(pSensorDesc->pDataCache,pDataBlob,dataBlobSize);

   // callbacks
   callAllCallbacks(pSensorDesc,EventType);
}
