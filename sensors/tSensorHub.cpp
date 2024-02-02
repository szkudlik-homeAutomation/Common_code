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
#include "../tMessageReciever.h"
#include "../TLE8457_serial/TLE8457_serial_lib.h"

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

       tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
       if (NULL != pSensorDesc)
       {
           DEBUG_PRINTLN_3("-----> Sensor already registgered ");
           return STATUS_DUPLICATE_ID;
       }

	   // add sensor to repository
	   pSensorDesc = tSensorFactory::Instance->CreateDesc(
			   pSensor->getSensorType(),
			   SensorID,
			   pSensorName,
			   pSensor->getSensorApiVersion(),
			   pSensor->getMeasurementBlobSize(),
			   0 /* local sensor */
			   );

	   // send response
	   DEBUG_PRINTLN_3("-----> DONE, SUCCESS");
	   return STATUS_SUCCESS;
}


uint8_t tSensorHub::RegisterRemoteSensor(uint8_t SensorID, uint8_t SensorType, uint8_t nodeID, char * pSensorName, uint8_t ApiVersion, uint8_t MeasurementBlobSize)
{
    DEBUG_PRINTLN_3("");
    DEBUG_PRINT_3("==>Sensor register request. ID: ");
    DEBUG_3(print(SensorID));
    DEBUG_PRINT_3(" on node: ");
    DEBUG_3(print(nodeID));
    DEBUG_PRINT_3(" name: ");
    DEBUG_3(println(pSensorName));

    tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
    if (NULL != pSensorDesc)
    {
        DEBUG_PRINTLN_3("-----> Sensor already registgered ");
        return STATUS_DUPLICATE_ID;
    }

    // add sensor to repository
    pSensorDesc = tSensorFactory::Instance->CreateDesc(
            SensorType,
            SensorID,
            pSensorName,
            ApiVersion,
            MeasurementBlobSize,
            nodeID);

    // send response
    DEBUG_PRINTLN_3("-----> DONE, SUCCESS");
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


void tSensorHub::HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message)
{
    tSensorDesc *pSensorDesc = tSensorDesc::getByID(Message->Header.SensorID);
    if (!pSensorDesc)
        return;

    // no data assembly?
    if (Message->Header.LastSegment && !pSensorDesc->pRemoteDataCache)
    {
    	onSensorEvent(Message->Header.SensorID, Message->Header.EventType, pSensorDesc->mDataBlobSize, Message->Payload);
    	return;
    }

    if (!pSensorDesc->pRemoteDataCache)
        return; // no space for packet reassemlby

    if (Message->Header.SegmentSeq != pSensorDesc->mSeq)
    {
    	// out of order
    	pSensorDesc->mSeq = 0;
    	return;
    }

    uint8_t toCopy = SENSOR_MEASUREMENT_PAYLOAD_SIZE;
    uint8_t offset = Message->Header.SegmentSeq * SENSOR_MEASUREMENT_PAYLOAD_SIZE;
    if (toCopy > pSensorDesc->mDataBlobSize - offset)
    {
    	toCopy = pSensorDesc->mDataBlobSize - offset;
    }
    memcpy((uint8_t *)pSensorDesc->pRemoteDataCache + offset, Message->Payload, toCopy);

    if (Message->Header.LastSegment)
    {
    	onSensorEvent(Message->Header.SensorID, Message->Header.EventType, pSensorDesc->mDataBlobSize, pSensorDesc->pRemoteDataCache);
    	pSensorDesc->mSeq = 0;
    }
    else
    {
    	pSensorDesc->mSeq++;
    }
}

void tSensorHub::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
{
#if CONFIG_SENSOR_HUB_GENERATE_EVENTS
   tSensorEvent Event;
#endif CONFIG_SENSOR_HUB_GENERATE_EVENTS

   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return;
   }

   if (EventType == EV_TYPE_MEASUREMENT_ERROR)
   {
      pSensorDesc->Status = STATUS_SENSOR_ERROR_REPORTED;
#if CONFIG_SENSOR_HUB_GENERATE_EVENTS
      Event.EventType = EV_TYPE_MEASUREMENT_ERROR;
      Event.SensorType = pSensorDesc->sensorType;
      Event.dataBlobSize = 0;
      Event.pDataBlob = NULL;
      tMessageReciever::Dispatch(MessageType_SensorEvent, SensorID, &Event);
#endif CONFIG_SENSOR_HUB_GENERATE_EVENTS
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

#if CONFIG_SENSOR_HUB_GENERATE_EVENTS
   Event.EventType = EventType;
   Event.dataBlobSize = pSensorDesc->mDataBlobSize;
   Event.pDataBlob = pSensorDesc->pDataCache;
   tMessageReciever::Dispatch(MessageType_SensorEvent, SensorID, &Event);
#endif CONFIG_SENSOR_HUB_GENERATE_EVENTS
}


void tSensorHub::onMessage(uint8_t type, uint16_t data, void *pData)
{
#if CONFIG_SENSORS_OVER_SERIAL_COMM

	if (type != MessageType_SerialFrameRecieved)
        return;

    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;
    uint8_t status = STATUS_SUCCESS;

    switch (data)   // messageType
    {
    case MESSAGE_TYPE_SENSOR_EVENT:
        HandleMsgSensorEvent(pFrame->SenderDevId, (tMessageSensorEvent *)pFrame->Data);
        break;
    }
#endif // CONFIG_SENSORS_OVER_SERIAL_COMM
}

#endif //CONFIG_SENSOR_HUB
