/*
 * IncomingFrameHandler.cpp
 *
 *  Created on: 10 sie 2022
 *      Author: szkud
 */


#include "../../global.h"

#include "IncomingFrameHandler.h"

#include "../../WorkerProcess.h"
//TODO

void IncomingFrameHandler::onFrame(void *pData, uint8_t MessageType, uint8_t SenderDevId)
{
      switch (MessageType)
      {
       case MESSAGE_TYPE_FW_VERSION_REQUEST:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FW_VERSION_REQUEST");
             HandleMsgVersionRequest(SenderDevId);
           break;

       case MESSAGE_TYPE_FW_VERSION_RESPONSE:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FW_VERSION_RESPONSE");
             HandleMsgVersionResponse(SenderDevId,(tMessageTypeFwVesionResponse*)(pData));
           break;

       case MESSAGE_TYPE_FORCE_RESET:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FORCE_RESET");
             cli();
             while(1); // let watchdog reboot the device
           break;

     default:
         DEBUG_PRINTLN_3("MESSAGE  unknown type, drop");
      }
}

void IncomingFrameHandler::HandleMsgVersionRequest(uint8_t SenderID)
{
   Worker.SendMsgVersionResponse(SenderID,FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH);
}


void IncomingFrameHandler::HandleMsgVersionResponse(uint8_t SenderID, tMessageTypeFwVesionResponse *Message)
{
#ifdef CONTROLLER
  RespHandler.VersionResponseHandler(SenderID,Message->Major,Message->Minor,Message->Patch);
#endif
}

