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

    virtual void onMessage(uint8_t type, uint16_t data, void *pData) { handleCommonMessages(data, pData); }

private:
    void HandleMsgVersionRequest(uint8_t SenderID);
    void HandleMsgVersionResponse(uint8_t SenderID, tMessageTypeFwVesionResponse *pMessage);
    void HandleMsgGeneralStatus(uint8_t SenderID, tMesssageGeneralStatus *pMessage);

#if CONFIG_OUTPUT_PROCESS
    void HandleMsgOverviewStateRequest(uint8_t SenderID);
    void HandleMsgOverviewStateResponse(uint8_t SenderID, tMessageTypeOverviewStateResponse* Message);
    void HandleMsgOutputStateRequest(uint8_t SenderID, tMessageTypeOutputStateRequest* Message);
    void HandleMsgOutputStateResponse(uint8_t SenderID, tMessageTypeOutputStateResponse* Message);
    void HandleMsgSetOutput(uint8_t SenderID, tMessageTypeSetOutput* Message);
#endif //CONFIG_OUTPUT_PROCESS
#if CONFIG_SENSORS
    void HandleMsgGetSensorByIdReqest(uint8_t SenderID, tMessageGetSensorByIdReqest *Message);
    void HandleMsgGetSensorByIdResponse(uint8_t SenderID, tMessageGetSensorByIdResponse *Message);
    void HandleMsgGetSensorMeasurementReqest(uint8_t SenderID, tMessageGetSensorMeasurementReqest *Message);
    void HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message);
    void HandleMsgSensorCreate(uint8_t SenderID, tMessageSensorCreate *Message);

#endif //CONFIG_SENSORS

protected:
    uint8_t handleCommonMessages(uint16_t data, void *pData);
};

#endif // CONFIG_TLE8457_COMM_LIB
