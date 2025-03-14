/*
 * tSensorHubMessageReciever.cpp
 *
 *  Created on: 12 lut 2024
 *      Author: szkud
 */

#include "tSensorHubMessageReciever.h"
#include "tSensorCache.h"
#include "../TLE8457_serial/CommonFramesDefs.h"
#include "../TLE8457_serial/TLE8457_serial_lib.h"
#include "../lib/strAllocateCopy.h"


#if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS

void tSensorHubMessageReciever::onMessage(uint8_t type, uint16_t data, void *pData)
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
void tSensorHubMessageReciever::HandleMsgSensorDetected(uint8_t SenderID, tMessageGetSensorByIdResponse *Message)
{
    // called every time when MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE is recieved
    // check if sensor cache for incoming sensor exits

    tSensorCache *pSensorCache = tSensorCache::getByID(Message->SensorID);
    if (NULL == pSensorCache)
    {
        // remote sensor seen for the first time
    	pSensorCache = new tSensorCache(Message->SensorID);
        if (NULL == pSensorCache)
        	// error??
        	return;
//TODO at this point sensor name need to be retrieved from a local dictionary
    	pSensorCache->setParams("REMOTE NAME", Message->SensorType, Message->ApiVersion, SenderID, Message->MeasurementBlobSize);
    }
    else
    {
        // sensor has been seen before. Check if the sensor looks identical as before
    	// skip local sensors

    	// TODO: check more deeply sensor state, runninh/stopped etc. Modify status
#if CONFIG_REMOTE_SENSORS_TEST
    	if (1 != Message->SensorID)
    		// in remote sensor testing all IDs > 1 are "remote"
#else
    	if (! pSensorCache->isLocalSensor())
#endif
    	{
            if ((pSensorCache->getSensorType() != Message->SensorType) ||
                (pSensorCache->getSensorApiVersion() != Message->ApiVersion) ||
                (pSensorCache->getNodeID() != SenderID))
            {
                pSensorCache->setError(tSensorCache::state_inconsistent_params);
            }
    	}
    }
}

void tSensorHubMessageReciever::HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message)
{
    uint8_t result;
	tSensorCache *pSensorCache = tSensorCache::getByID(Message->Header.SensorID);
	if (!pSensorCache) {
		// the sensor is not yet know. Send discovery request
	    tMessageGetSensorByIdReqest getSensorMessage;
	    getSensorMessage.SensorID = Message->Header.SensorID;
	    DEBUG_PRINTLN_2("SENDING MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST");
	    CommSenderProcess::Instance->Enqueue(SenderID, MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST, sizeof(getSensorMessage), &getSensorMessage);
		return;
	}

	// no data assembly?
	if (Message->Header.LastSegment && Message->Header.SegmentSeq == 0 && !pSensorCache->isDataAssemblyNeeded())
	{
		tSensorHub::Instance->onSensorEvent(Message->Header.SensorID, Message->Header.EventType, pSensorCache->getDataBlobSize(), Message->Payload);
		return;
	}

	result = pSensorCache->addDataSegment(Message->Header.SegmentSeq, Message->Payload);
	if (STATUS_SUCCESS != result)
	{
	    // error state for a sensor has already been set
	    return;
	}

	if (Message->Header.LastSegment)
	{
	    tSensorHub::Instance->onSensorEvent(Message->Header.SensorID, Message->Header.EventType, pSensorCache->getDataBlobSize(), pSensorCache->getAssembledData());
        pSensorCache->resetDataSegment();
	}
}

#endif //CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
