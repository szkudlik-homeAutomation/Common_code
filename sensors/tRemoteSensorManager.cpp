/*
 * tRemoteSensorManager.cpp
 *
 *  Created on: 3 lut 2024
 *      Author: szkud
 */

#include "../../../global.h"
#if CONFIG_SENSORS_OVER_SERIAL_COMM


#include "tRemoteSensorManager.h"
#include "tSensorCache.h"
#include "tSensorHub.h"
#include "../TLE8457_serial/TLE8457_serial_lib.h"

void tRemoteSensorManager::onMessage(uint8_t type, uint16_t data, void *pData)
{
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
}

void tRemoteSensorManager::HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message)
{
    tSensorCache *pSensorCache = tSensorCache::getByID(Message->Header.SensorID);
    if (!pSensorCache)
        return;

    // no data assembly?
    if (Message->Header.LastSegment && !pSensorCache->pRemoteDataCache)
    {
    	tSensorHub::Instance->onSensorEvent(Message->Header.SensorID, Message->Header.EventType, pSensorCache->getDataBlobSize(), Message->Payload);
    	return;
    }

    if (!pSensorCache->pRemoteDataCache)
        return; // no space for packet reassemlby

    if (Message->Header.SegmentSeq != pSensorCache->mSeq)
    {
    	// out of order
    	pSensorCache->mSeq = 0;
    	return;
    }

    uint8_t toCopy = SENSOR_MEASUREMENT_PAYLOAD_SIZE;
    uint8_t offset = Message->Header.SegmentSeq * SENSOR_MEASUREMENT_PAYLOAD_SIZE;
    if (toCopy > pSensorCache->getDataBlobSize() - offset)
    {
    	toCopy = pSensorCache->getDataBlobSize() - offset;
    }
    memcpy((uint8_t *)pSensorCache->pRemoteDataCache + offset, Message->Payload, toCopy);

    if (Message->Header.LastSegment)
    {
    	tSensorHub::Instance->onSensorEvent(Message->Header.SensorID, Message->Header.EventType, pSensorCache->getDataBlobSize(), pSensorCache->pRemoteDataCache);
    	pSensorCache->mSeq = 0;
    }
    else
    {
    	pSensorCache->mSeq++;
    }
}

#endif //CONFIG_SENSORS_OVER_SERIAL_COMM
