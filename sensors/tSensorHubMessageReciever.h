/*
 * tSensorHubMessageReciever.h
 *
 *  Created on: 12 lut 2024
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"

#if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS

#include "tSensorHub.h"
#include "../tMessageReciever.h"
#include "../TLE8457_serial/CommonFramesDefs.h"

class tSensorHubMessageReciever: public tMessageReciever {
public:
	tSensorHubMessageReciever()
	{
		RegisterMessageType(MessageType_SerialFrameRecieved);
	}
protected:
   virtual void onMessage(uint8_t type, uint16_t data, void *pData);
private:
   void HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message);
   void HandleMsgSensorDetected(uint8_t SenderID, tMessageGetSensorByIdResponse *Message);
};

#endif // CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
