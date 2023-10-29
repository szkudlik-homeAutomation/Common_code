/*
 * tIncomingFrameHanlder.cpp
 *
 *  Created on: Oct 10, 2023
 *      Author: mszkudli
 */

#include "tIncomingFrameHanlder.h"
#include "tOutgoingFrames.h"
#if CONFIG_OUTPUT_PROCESS
#include "../OutputProcess.h"
#endif // CONFIG_OUTPUT_PROCESS

uint8_t tIncomingFrameHanlder::handleCommonMessages(uint8_t type, uint16_t data, void *pData)
{
    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;
    uint8_t status = STATUS_SUCCESS;

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
           HandleMsgOverviewStateRequest(SenderDevId);
           break;

       case MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE");
           HandleMsgOverviewStateResponse(SenderDevId,(tMessageTypeOverviewStateResponse*) (pFrame->Data));
           break;

       case MESSAGE_TYPE_OUTPUT_STATE_REQUEST:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OUTPUT_STATE_REQUEST");
           HandleMsgOutputStateRequest(SenderDevId,(tMessageTypeOutputStateRequest*)(pFrame->Data));
           break;

       case MESSAGE_TYPE_OUTPUT_STATE_RESPONSE:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OUTPUT_STATE_RESPONSE");
           HandleMsgOutputStateResponse(SenderDevId,(tMessageTypeOutputStateResponse*) (pFrame->Data));
           break;

       case MESSAGE_TYPE_SET_OUTPUT:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_SET_OUTPUT");
           HandleMsgSetOutput(SenderDevId,(tMessageTypeSetOutput*)(pFrame->Data));
           break;
#endif //CONFIG_OUTPUT_PROCESS

       default:
           status = STATUS_UNKNOWN_MESSAGE;
    }

    return status;
}

void tIncomingFrameHanlder::HandleMsgVersionRequest(uint8_t SenderID)
{
    tOutgoingFrames::SendMsgVersionResponse(SenderID,FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH);
}

void tIncomingFrameHanlder::HandleMsgVersionResponse(uint8_t SenderID, tMessageTypeFwVesionResponse *Message)
{
#if CONFIG_CENTRAL_NODE
    tMessages::VersionResponseHandler(SenderID,Message->Major,Message->Minor,Message->Patch);
#endif
}


#if CONFIG_OUTPUT_PROCESS
void tIncomingFrameHanlder::HandleMsgOverviewStateRequest(uint8_t SenderID)
{
   tOutgoingFrames::SendMsgOverviewStateResponse(SenderID,tOutputProcess::get()->GetOutputStateMap(),tOutputProcess::get()->GetOutputTimersStateMap());
}


void tIncomingFrameHanlder::HandleMsgOverviewStateResponse(uint8_t SenderID, tMessageTypeOverviewStateResponse* Message)
{
#if CONFIG_CENTRAL_NODE
	tMessages::OverviewStateResponseHandler(SenderID,Message->PowerState,Message->TimerState);
#endif
}

void tIncomingFrameHanlder::HandleMsgOutputStateRequest(uint8_t SenderID, tMessageTypeOutputStateRequest* Message)
{
  if (Message->OutputID < NUM_OF_OUTPUTS)
  {
      uint16_t DefTimer;
#if CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER
      EEPROM.get(EEPROM_DEFAULT_TIMER_VALUE_OFFSET+Message->OutputID*(sizeof(uint16_t)),DefTimer);
#else CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER
      DefTimer = 0;
#endif CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER

      tOutgoingFrames::SendMsgOutputStateResponse(SenderID,Message->OutputID, tOutputProcess::get()->GetOutputState(Message->OutputID), tOutputProcess::get()->GetOutputTimer(Message->OutputID),DefTimer);
  }
}

void tIncomingFrameHanlder::HandleMsgOutputStateResponse(uint8_t SenderID, tMessageTypeOutputStateResponse* Message)
{
#if CONFIG_CENTRAL_NODE
	tMessages::OutputStateResponseHandler(SenderID,Message->OutputID,Message->PowerState,Message->TimerValue,Message->DefaultTimer);
#endif
}

void tIncomingFrameHanlder::HandleMsgSetOutput(uint8_t SenderID, tMessageTypeSetOutput* Message)
{
   if (Message->OutId >= NUM_OF_OUTPUTS)
   {
     // drop it
     return;
   }
   uint16_t Timer = Message->Timer;

#if CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER
   if (DEFAULT_TIMER == Timer)
   {
     EEPROM.get(EEPROM_DEFAULT_TIMER_VALUE_OFFSET+Message->OutId*(sizeof(uint16_t)),Timer);
   }
#endif // CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER

  tOutputProcess::get()->SetOutput(Message->OutId,Message->State,Timer,tOutputProcess::ForceTimer);
}

#endif // CONFIG_OUTPUT_PROCESS
