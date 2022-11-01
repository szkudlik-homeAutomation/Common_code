/*
 * tSensorHub.cpp
 *
 *  Created on: Oct 28, 2022
 *      Author: mszkudli
 */

#include "tSensorHub.h"
#include "tSensor.h"

tSensorDesc * tSensorDesc::pFirst = NULL;


tSensorDesc *tSensorDesc::getByID(uint8_t SensorID)
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

tSensorDesc *tSensorDesc::getByName(const char * pSensorName)
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

   Status = pSensor->SetSpecificConfig(pConfigBlob);
   if (Status != CREATE_SENSOR_STATUS_OK)
   {
      CreateSensorResponse(SensorID, Status);
      return;
   }

   pSensor->SetMeasurementPeriod(MeasurementPeriod);

   // add sensor to repository
   tSensorDesc *pSensorDesc = new tSensorDesc;
   pSensorDesc->SensorID = SensorID;
   pSensorDesc->pDataCache = NULL;
   pSensorDesc->dataBlobSize = 0;
   pSensorDesc->pName = pSensorName;
   pSensorDesc->sensorType = SensorType;

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
   // note that the sensor may be located on a remote machine, use cached data
   return tSensor::TranslateBlobToJSON(pSensorDesc->sensorType, pSensorDesc->dataBlobSize, pSensorDesc->pDataCache, pStream);
}
