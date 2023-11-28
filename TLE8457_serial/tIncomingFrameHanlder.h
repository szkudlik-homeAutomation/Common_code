/*
 * tIncomingFrameHanlder.h
 *
 *  Created on: Oct 10, 2023
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"
#if CONFIG_TLE8457_COMM_LIB


#include "TLE8457_serial_lib.h"
#include "../tMessageReciever.h"
#include "../tMessages.h"

class tIncomingFrameHanlder : public tMessageReciever {
public:
    tIncomingFrameHanlder() : tMessageReciever() { RegisterMessageType(tMessages::MessageType_SerialFrameRecieved); }

    virtual void onMessage(uint8_t type, uint16_t data, void *pData);

private:
    void HandleMsgVersionRequest(uint8_t SenderID);
    void HandleMsgVersionResponse(uint8_t SenderID, tMessageTypeFwVesionResponse *pMessage);
    void LogMsgGeneralStatus(uint8_t SenderID, tMesssageGeneralStatus *pMessage);

#if CONFIG_OUTPUT_PROCESS
    void LogMsgOverviewStateResponse(uint8_t SenderID, tMessageTypeOverviewStateResponse* Message);
    void LogMsgOutputStateResponse(uint8_t SenderID, tMessageTypeOutputStateResponse* Message);
#endif //CONFIG_OUTPUT_PROCESS

};

#endif // CONFIG_TLE8457_COMM_LIB
