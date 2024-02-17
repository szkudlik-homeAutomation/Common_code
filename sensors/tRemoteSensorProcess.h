/*
 * tRemoteSensorProcess.h
 *
 *  Created on: 13 lut 2024
 *      Author: szkud
 */

#pragma	 once

#include "../../../global.h"

#if CONFIG_SENSORS_OVER_SERIAL_COMM

#include "tSensor.h"
#include "../tMessageReciever.h"
#include "../TLE8457_serial/CommonFramesDefs.h"


class tRemoteSensorProcess: public tSensorProcess, public tMessageReciever {
public:
	tRemoteSensorProcess(Scheduler &manager) : tSensorProcess(manager)
	   { RegisterMessageType(MessageType_SerialFrameRecieved); }

protected:
    virtual void onMessage(uint8_t type, uint16_t data, void *pData);

private:
    void HandleMessageGetSensorByIdReqest(uint8_t sender, tMessageGetSensorByIdReqest *pFrame);
    void HandleMsgSensorCreate(uint8_t sender, tMessageSensorCreate *pFrame);
    void HandleMsgSensorConfigure(uint8_t SenderID, tMessageSensorConfigure *Message);
    void HandleMsgSensorStart(uint8_t SenderID, tMessageSensorStart *Message);
    void HandleMsgSensorStop(uint8_t SenderID, tMessageSensorStop *Message);
    void HandleMsgGetSensorMeasurementReqest(uint8_t SenderID, tMessageGetSensorMeasurementReqest *Message);
};

#endif /* CONFIG_SENSORS_OVER_SERIAL_COMM */
