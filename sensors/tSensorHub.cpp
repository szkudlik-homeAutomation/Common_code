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

   *dataBlobSize = pSensorDesc->dataBlobSize;
   *pDataBlob = pSensorDesc->pDataCache;
}

void tSensorHub::CreateSensorRequest(uint8_t TargetNode, uint8_t SensorType, uint8_t SensorID, char * pSensorName, void* pConfigBlob, uint8_t MeasurementPeriod)
{
   //TODO: async sensor creation on external nodes

   uint8_t Status;
   (void) TargetNode; // not yet implemented

   // create a sensor on a local node

   DEBUG_PRINTLN_3("");
   DEBUG_PRINT_3("==>Sensor create request. ID: ");
   DEBUG_3(print(SensorID));
   DEBUG_PRINT_3(" at node: ");
   DEBUG_3(print(TargetNode));
   DEBUG_PRINT_3(" name: ");
   DEBUG_3(println(pSensorName));

   Status = tSensor::Create(SensorType, SensorID);
   if (Status != STATUS_SUCCESS)
   {
      DEBUG_PRINT_3("-----> tSensor::Create error, status: ");
      DEBUG_3(println(Status));
      CreateSensorResponse(SensorID, Status);
      return;
   }

   tSensor *pSensor = tSensor::getSensor(SensorID);
   if (pSensor == NULL)
   {
      DEBUG_PRINTLN_3("-----> tSensor::getSensor error ");
      CreateSensorResponse(SensorID, STATUS_SENSOR_CREATE_ERROR);
      return;
   }

   if (NULL != pConfigBlob)
   {
      DEBUG_PRINTLN_3("Setting sensor config ");
      Status = pSensor->SetSpecificConfig(pConfigBlob);
      if (Status != STATUS_SUCCESS)
      {
         DEBUG_PRINT_3("-----> tSensor::SetSpecificConfig error, status:");
         DEBUG_3(println(Status));
         CreateSensorResponse(SensorID, Status);
         return;
      }
   }
   pSensor->SetMeasurementPeriod(MeasurementPeriod);

   // add sensor to repository
   tSensorDesc *pSensorDesc = new tSensorDesc(
		   SensorType,
		   SensorID,
		   pSensorName
		   );

   pSensor->SetEventCalback(this);
   // send response
   DEBUG_PRINTLN_3("-----> DONE, SUCCESS");
   CreateSensorResponse(SensorID, STATUS_SUCCESS);
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

uint8_t tSensorHub::getCachedSensorDataJson(uint8_t SensorID, Stream *pStream)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
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
      pStream->print("\r\n");
   }
}

uint8_t tSensorHub::formatJSON(tSensorDesc *pSensorDesc, Stream *pStream)
{
   uint8_t Result;
   // note that the sensor may be located on a remote machine, use cached data
   pStream->print(F("\""));
   pStream->print(pSensorDesc->pName);
   pStream->print(F("\":{"));
   if (pSensorDesc->Status == STATUS_SUCCESS)
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
   pStream->print(F("}"));
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
      pSensorDesc->Status = STATUS_SENSOR_ERROR_REPORTED;
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
      pSensorDesc->Status = STATUS_INCORRECT_DATA_SIZE;
      callAllCallbacks(pSensorDesc,EV_TYPE_MEASUREMENT_ERROR);
      return;
   }

   pSensorDesc->Status = STATUS_SUCCESS;
   memcpy(pSensorDesc->pDataCache,pDataBlob,dataBlobSize);

   // callbacks
   callAllCallbacks(pSensorDesc,EventType);
}
