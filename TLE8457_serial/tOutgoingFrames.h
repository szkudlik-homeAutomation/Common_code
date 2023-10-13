/*
 * tOutgoingFrames.h
 *
 *  Created on: Oct 10, 2023
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"

#if CONFIG_TLE8457_COMM_LIB

class tOutgoingFrames {
public:
    tOutgoingFrames() {}

    static bool SendMsgVersionRequest(uint8_t RecieverID);
    static bool SendMsgVersionResponse(uint8_t RecieverID, uint8_t Major, uint8_t Minor, uint8_t Patch);

};

#endif /* CONFIG_TLE8457_COMM_LIB */
