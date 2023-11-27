#pragma once
/*
 * a handler of messages to be used in all applications
 * uses tMessageReciever to dispatch messages
 */
#include "../../global.h"

class tMessages {
public:

	/*
	 * a frame has been recieved - an event dedicated for serial frame parser
	 *
	 *  data is always mFrame.MessageType
	 */
	static const uint8_t MessageType_SerialFrameRecieved = 0;

	static const uint8_t MessageType_SensorEvent   = 2;

	// NOTE - app specific event types start from 16


};
