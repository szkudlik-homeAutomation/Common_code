/*
 * tIncomingFrameHanlder.cpp
 *
 *  Created on: Oct 10, 2023
 *      Author: mszkudli
 */

#include "../../../global.h"
#if CONFIG_TLE8457_COMM_LIB

#include "tIncomingFrameHanlder.h"
#include "tOutgoingFrames.h"

void tIncomingFrameHanlder::onMessage(uint8_t type, uint16_t data, void *pData)
{
    if (type != MessageType_SerialFrameRecieved)
        return;

    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;

    switch (data)   // messageType
    {
       case MESSAGE_TYPE_FW_VERSION_REQUEST:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FW_VERSION_REQUEST");
             HandleMsgVersionRequest(SenderDevId);
           break;

       case MESSAGE_TYPE_FW_VERSION_RESPONSE:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FW_VERSION_RESPONSE");
             HandleMsgVersionResponse(SenderDevId,(tMessageTypeFwVesionResponse*)(pFrame->Data));
           break;

       case MESSAGE_TYPE_FORCE_RESET:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FORCE_RESET");
             cli();
             while(1); // let watchdog reboot the device
           break;
       case MESSAGE_TYPE_GENERAL_STATUS:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_GENERAL_STATUS");
           LogMsgGeneralStatus(SenderDevId, (tMesssageGeneralStatus *)(pFrame->Data));
           break;

#if CONFIG_OUTPUT_PROCESS
       case MESSAGE_TYPE_OVERVIEW_STATE_REQUEST:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OVERVIEW_STATE_REQUEST");
           break;

       case MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE:
           LogMsgOverviewStateResponse(SenderDevId,(tMessageTypeOverviewStateResponse*) (pFrame->Data));
           break;

       case MESSAGE_TYPE_OUTPUT_STATE_REQUEST:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OUTPUT_STATE_REQUEST");
           break;

       case MESSAGE_TYPE_OUTPUT_STATE_RESPONSE:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OUTPUT_STATE_RESPONSE");
           LogMsgOutputStateResponse(SenderDevId,(tMessageTypeOutputStateResponse*) (pFrame->Data));
           break;

       case MESSAGE_TYPE_SET_OUTPUT:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_SET_OUTPUT");
           break;
#endif //CONFIG_OUTPUT_PROCESS
#if CONFIG_SENSORS
       case MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST");
           break;

       case MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE");
           LogMsgGetSensorByIdResponse(SenderDevId, (tMessageGetSensorByIdResponse*)(pFrame->Data));
           break;
#endif //CONFIG_SENSORS

    }
}

void tIncomingFrameHanlder::HandleMsgVersionRequest(uint8_t SenderID)
{
    tOutgoingFrames::SendMsgVersionResponse(SenderID,FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH);
}

void tIncomingFrameHanlder::HandleMsgVersionResponse(uint8_t SenderID, tMessageTypeFwVesionResponse *pMessage)
{
	LOG_PRINT("FW Version for device ");
	LOG(print(SenderID,HEX));
	LOG_PRINT("=");
	LOG(print(pMessage->Major,DEC));
	LOG_PRINT(".");
	LOG(print(pMessage->Minor,DEC));
	LOG_PRINT(".");
	LOG(println(pMessage->Patch,DEC));
}

void tIncomingFrameHanlder::LogMsgGeneralStatus(uint8_t SenderID, tMesssageGeneralStatus *pMessage)
{
    LOG_PRINT("Message from node: ");
    LOG(print(SenderID,HEX));
    LOG_PRINT(" status: ");
    LOG(println(pMessage->Status,DEC));
}

#if CONFIG_OUTPUT_PROCESS

void tIncomingFrameHanlder::LogMsgOverviewStateResponse(uint8_t SenderID, tMessageTypeOverviewStateResponse* Message)
{
    DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE");
	LOG_PRINT("PowerStateBitmap for device ");
	LOG(print(SenderID,HEX));
	LOG_PRINT("=");
	LOG(print(Message->PowerState,BIN));
	LOG_PRINT(" with timers map=");
	LOG(println(Message->TimerState,BIN));
}

void tIncomingFrameHanlder::LogMsgOutputStateResponse(uint8_t SenderID, tMessageTypeOutputStateResponse* Message)
{
	LOG_PRINT("PowerState for device ");
	LOG(print(SenderID,HEX));
	LOG_PRINT(" output ID ");
	LOG(print(Message->OutputID,DEC));
	LOG_PRINT("=");
	LOG(print(Message->PowerState,DEC));
	LOG_PRINT(" with timers = ");
	LOG(print(Message->TimerValue,DEC));
    LOG_PRINT(" default timer = ");
    LOG(println(Message->DefaultTimer,DEC));
}

#endif // CONFIG_OUTPUT_PROCESS

#if CONFIG_SENSORS
void tIncomingFrameHanlder::LogMsgGetSensorByIdResponse(uint8_t SenderID, tMessageGetSensorByIdResponse *Message)
{
	LOG_PRINT("Sensor ID ");
	LOG(print(Message->SensorID, DEC));
	LOG_PRINT(" found on node ");
	LOG(println(SenderID,DEC));
	LOG_PRINT(" ->sensor type: ");
	LOG(println(Message->SensorType,DEC));
	LOG_PRINT(" ->API version: ");
	LOG(println(Message->ApiVersion,DEC));
	LOG_PRINT(" ->MeasurementPeriod: ");
	LOG(println(Message->MeasurementPeriod,DEC));
	LOG_PRINT(" ->Config blob size: ");
	LOG(println(Message->ConfigBlobSize,DEC));
	LOG_PRINT(" ->measurement blob size: ");
	LOG(println(Message->MeasurementBlobSize,DEC));
	LOG_PRINT(" ->isConfigured: ");
	LOG(print(Message->isConfigured,DEC));
	LOG_PRINT(" isRunning: ");
	LOG(print(Message->isRunning,DEC));
	LOG_PRINT(" isMeasurementValid: ");
	LOG(print(Message->isMeasurementValid,DEC));
	LOG_PRINT(" EventMask: ");
	LOG(println(Message->EventsMask,BIN));
	   //TODO: send a message
}
#endif // CONFIG_SENSORS
#endif // CONFIG_TLE8457_COMM_LIB
