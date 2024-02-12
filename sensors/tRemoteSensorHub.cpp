/*
 * tRemoteSensorHub.cpp
 *
 *  Created on: 12 lut 2024
 *      Author: szkud
 */

#include "tRemoteSensorHub.h"
#include "tSensorCache.h"
#include "../TLE8457_serial/CommonFramesDefs.h"
#include "../TLE8457_serial/TLE8457_serial_lib.h"

#if CONFIG_SENSORS_OVER_SERIAL_COMM

void tRemoteSensorHub::onMessage(uint8_t type, uint16_t data, void *pData)
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
    case MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE:
        HandleMsgSensorDetected(pFrame->SenderDevId, (tMessageGetSensorByIdResponse *)pFrame->Data);
        break;
    }
}
void tRemoteSensorHub::HandleMsgSensorDetected(uint8_t SenderID, tMessageGetSensorByIdResponse *Message)
{
}

void tRemoteSensorHub::HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message)
{
	tSensorCache *pSensorCache = tSensorCache::getByID(Message->Header.SensorID);
	if (!pSensorCache)
		return;


	if (Message->Header.LastSegment)
	{
		onSensorEvent(Message->Header.SensorID, Message->Header.EventType, pSensorCache->getDataBlobSize(), Message->Payload);
		return;
	}
}

#endif //CONFIG_SENSORS_OVER_SERIAL_COMM
