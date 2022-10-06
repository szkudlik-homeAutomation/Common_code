/*
 * IncomingFrameHandler.h
 *
 *  Created on: 10 sie 2022
 *      Author: szkud
 */

#ifndef SRC_INCOMINGFRAMEHANDLER_H_
#define SRC_INCOMINGFRAMEHANDLER_H_

#include "../global.h"
#include "lib/TLE8457_serial_lib/TLE8457_serial_lib.h"

class IncomingFrameHandler: public CommRecieverProcessCallback {
public:
   IncomingFrameHandler() {};

   virtual void onFrame(void *pData, uint8_t MessageType, uint8_t SenderDevId);

private:
   // handlers of all incoming frames

   void HandleMsgVersionRequest(uint8_t SenderID);
   void HandleMsgVersionResponse(uint8_t SenderID, tMessageTypeFwVesionResponse *Message);
};


#endif /* SRC_INCOMINGFRAMEHANDLER_H_ */
