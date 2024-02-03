/*
 * tRemoteSensorManager.h
 *
 *  Created on: 3 lut 2024
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"

#if CONFIG_SENSORS_OVER_SERIAL_COMM

#include "../tMessageReciever.h"
#include "../TLE8457_serial/CommonFramesDefs.h"

class tRemoteSensorManager : public tMessageReciever {
public:
	tRemoteSensorManager()
	{
		RegisterMessageType(MessageType_SerialFrameRecieved);
	}

protected:
   virtual void onMessage(uint8_t type, uint16_t data, void *pData);
private:
   void HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message);
};


#endif // CONFIG_SENSORS_OVER_SERIAL_COMM
