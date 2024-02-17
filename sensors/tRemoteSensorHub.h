/*
 * tRemoteSensorHub.h
 *
 *  Created on: 12 lut 2024
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"

#if CONFIG_SENSORS_OVER_SERIAL_COMM

#include "tSensorHub.h"
#include "../tMessageReciever.h"
#include "../TLE8457_serial/CommonFramesDefs.h"

class tRemoteSensorHub: public tSensorHub, public tMessageReciever {
public:
	tRemoteSensorHub() : tSensorHub()
	{
		RegisterMessageType(MessageType_SerialFrameRecieved);
	}
protected:
   virtual void onMessage(uint8_t type, uint16_t data, void *pData);
private:
   void HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message);
   void HandleMsgSensorDetected(uint8_t SenderID, tMessageGetSensorByIdResponse *Message);
};

#endif // CONFIG_SENSORS_OVER_SERIAL_COMM
