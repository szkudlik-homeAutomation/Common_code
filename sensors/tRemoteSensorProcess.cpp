/*
 * tRemoteSensorProcess.cpp
 *
 *  Created on: 13 lut 2024
 *      Author: szkud
 */

#include "../../../global.h"
#if CONFIG_SENSORS_OVER_SERIAL_COMM

#include "tRemoteSensorProcess.h"
#include "tSensorFactory.h"
#include "../tMessageReciever.h"
#include "../TLE8457_serial/TLE8457_serial_lib.h"
#include "../TLE8457_serial/tOutgoingFrames.h"


void tRemoteSensorProcess::onMessage(uint8_t type, uint16_t data, void *pData)
{
    if (type != MessageType_SerialFrameRecieved)
        return;

    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;
    uint8_t status = STATUS_SUCCESS;

    switch (data)   // messageType
    {
    case MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST:
        HandleMessageGetSensorByIdReqest(pFrame->SenderDevId, (tMessageGetSensorByIdReqest *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_CREATE:
        HandleMsgSensorCreate(pFrame->SenderDevId, (tMessageSensorCreate *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_CONFIGURE:
        HandleMsgSensorConfigure(pFrame->SenderDevId, (tMessageSensorConfigure *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_START:
        HandleMsgSensorStart(pFrame->SenderDevId, (tMessageSensorStart *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_STOP:
        HandleMsgSensorStop(pFrame->SenderDevId, (tMessageSensorStop *)pFrame->Data);
        break;
    case MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST:
        HandleMsgGetSensorMeasurementReqest(SenderDevId,(tMessageGetSensorMeasurementReqest*)(pFrame->Data));
        break;
    }
}

void tRemoteSensorProcess::HandleMessageGetSensorByIdReqest(uint8_t sender, tMessageGetSensorByIdReqest *pFrame)
{
    tSensor *pSensor = tSensor::getSensor(pFrame->SensorID);
    if (NULL != pSensor)
    {
          tMessageGetSensorByIdResponse Response;
          Response.SensorID = pFrame->SensorID;
          Response.MeasurementPeriod = pSensor->GetMeasurementPeriod();
          Response.ApiVersion = pSensor->getSensorApiVersion();
          Response.SensorType = pSensor->getSensorType();
          Response.isConfigured = pSensor->isConfigured();
          Response.isMeasurementValid = pSensor->isMeasurementValid();
          Response.isRunning = pSensor->isRunning();
          Response.EventsMask = pSensor->getSensorSerialEventsMask();
          Response.ConfigBlobSize = pSensor->getConfigBlobSize();
          Response.MeasurementBlobSize = pSensor->getMeasurementBlobSize();
          CommSenderProcess::Instance->Enqueue(sender, MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE, sizeof(Response), &Response);
    }
}

void tRemoteSensorProcess::HandleMsgSensorCreate(uint8_t sender, tMessageSensorCreate *pFrame)
{
    DEBUG_PRINT_3("Creating type: ");
    DEBUG_3(print(pFrame->SensorType, DEC));
    DEBUG_PRINT_3(" with ID: ");
    DEBUG_3(println(pFrame->SensorID, DEC));

    tSensor *pSensor = tSensorFactory::Instance->CreateSensor(pFrame->SensorType, pFrame->SensorID,"REMOTE-todo");

    if (pSensor)
        tOutgoingFrames::SendMsgStatus(sender, 0);
    else
        tOutgoingFrames::SendMsgStatus(sender, STATUS_GENERAL_FAILURE);
}

void tRemoteSensorProcess::HandleMsgSensorConfigure(uint8_t SenderID, tMessageSensorConfigure *Message)
{
    uint8_t result;
    tSensor *pSensor = tSensor::getSensor(Message->Header.SensorID);
    if (!pSensor)
        return;

    DEBUG_PRINT_3("Setting config for sensor ID: ");
    DEBUG_3(println(Message->Header.SensorID, DEC));

    if (Message->Header.LastSegment == 1)
    {
        result = pSensor->setConfig(Message->Data.MeasurementPeriod);
        tOutgoingFrames::SendMsgStatus(SenderID, result);
    }
    else
    {
        result = pSensor->setParitalConfig(Message->Header.SegmentSeq, Message->Payload, SENSOR_CONFIG_PAYLOAD_SIZE);
        if (result != STATUS_SUCCESS)
        {
            tOutgoingFrames::SendMsgStatus(SenderID, result);
        }
    }
}


void tRemoteSensorProcess::HandleMsgSensorStart(uint8_t SenderID, tMessageSensorStart *Message)
{
    tSensor *pSensor = tSensor::getSensor(Message->SensorID);
    if (!pSensor)
        return;

    DEBUG_PRINT_3("Starting sensor ID: ");
    DEBUG_3(print(Message->SensorID, DEC));
    DEBUG_PRINT_3(" with event mask: ");
    DEBUG_3(println(Message->SensorEventMask, BIN));

    uint8_t result = pSensor->Start();
    if (STATUS_SUCCESS == result)
        pSensor->setSensorSerialEventsMask(Message->SensorEventMask);

    DEBUG_PRINT_3("   status: ");
    DEBUG_3(println(result, DEC));

    tOutgoingFrames::SendMsgStatus(SenderID, result);
}

void tRemoteSensorProcess::HandleMsgSensorStop(uint8_t SenderID, tMessageSensorStop *Message)
{
    tSensor *pSensor = tSensor::getSensor(Message->SensorID);
    if (!pSensor)
        return;

    DEBUG_PRINT_3("Stopping sensor ID: ");
    DEBUG_3(println(Message->SensorID, DEC));

    uint8_t result = pSensor->Pause();

    DEBUG_PRINT_3("   status: ");
    DEBUG_3(println(result, DEC));

    tOutgoingFrames::SendMsgStatus(SenderID, result);
}

void tRemoteSensorProcess::HandleMsgGetSensorMeasurementReqest(uint8_t SenderID, tMessageGetSensorMeasurementReqest *Message)
{
    /* get data blob from sensor and send it in one or more frames */
    tSensor *pSensor = tSensor::getSensor(Message->SensorID);
    if (NULL == pSensor)
        return;

    pSensor->sendSerialMsgSensorEvent(true, EV_TYPE_MEASUREMENT_COMPLETED);
}


#endif // CONFIG_SENSORS_OVER_SERIAL_COMM
