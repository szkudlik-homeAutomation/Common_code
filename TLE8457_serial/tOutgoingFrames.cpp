/*
 * tOutgoingFrames.cpp
 *
 *  Created on: Oct 10, 2023
 *      Author: mszkudli
 */

#include "tOutgoingFrames.h"
#include "TLE8457_serial_lib.h"

// VERSION HANDSHAKE
bool tOutgoingFrames::SendMsgVersionRequest(uint8_t RecieverID)
{
#if CONFIG_CENTRAL_NODE
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_FW_VERSION_REQUEST");
  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_FW_VERSION_REQUEST,0,NULL);
#endif
  return true;
}

bool tOutgoingFrames::SendMsgVersionResponse(uint8_t RecieverID, uint8_t Major, uint8_t Minor, uint8_t Patch)
{
  tMessageTypeFwVesionResponse Msg;
  Msg.Major = Major;
  Msg.Minor = Minor;
  Msg.Patch = Patch;

  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_FW_VERSION_RESPONSE,sizeof(Msg),&Msg);
};

bool tOutgoingFrames::SendMsgReset(uint8_t RecieverID)
{
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_FORCE_RESET");
  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_FORCE_RESET,0,NULL);
}

