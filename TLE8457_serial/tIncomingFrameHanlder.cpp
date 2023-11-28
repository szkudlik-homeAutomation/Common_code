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
#endif // CONFIG_TLE8457_COMM_LIB
