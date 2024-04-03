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
#include "../lib/strAllocateCopy.h"


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
    // called every time when MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE is recieved
    // check if sensor cache for incoming sensor exits

    tSensorCache *pSensorCache = tSensorCache::getByID(Message->Header.SensorID);
    if (NULL == pSensorCache)
    {
        // remote sensor seen for the first time
    	pSensorCache = new tSensorCache(Message->Header.SensorID);
        if (NULL == pSensorCache)
        	// error??
        	return;

    	// make copy of sensor name
        char *nameCopy = strAllocateCopy(Message->name, sizeof(Message->name));

    	pSensorCache->setParams(nameCopy, Message->Header.SensorType, Message->Header.ApiVersion, SenderID, Message->Header.MeasurementBlobSize);
    }
    else
    {
        // sensor has been seen before. Check
    	// skip local sensors
#if REMOTE_SENSORS_TEST
    	if (1 != Message->Header.SensorID)
#else
    	if (! pSensorCache->isLocalSensor())
#endif
    	{
            if ((pSensorCache->getSensorType() != Message->Header.SensorType) ||
                (pSensorCache->getSensorApiVersion() != Message->Header.ApiVersion) ||
                (pSensorCache->getNodeID() != SenderID))
            {
                pSensorCache->setError(tSensorCache::state_inconsistent_params);
            }
    	}
    }
}

void tRemoteSensorHub::HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message)
{
    uint8_t result;
	tSensorCache *pSensorCache = tSensorCache::getByID(Message->Header.SensorID);
	if (!pSensorCache)
		return;

	// no data assembly?
	if (Message->Header.LastSegment && Message->Header.SegmentSeq == 0 && !pSensorCache->isDataAssemblyNeeded())
	{
		onSensorEvent(Message->Header.SensorID, Message->Header.EventType, pSensorCache->getDataBlobSize(), Message->Payload);
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

#endif //CONFIG_SENSORS_OVER_SERIAL_COMM
