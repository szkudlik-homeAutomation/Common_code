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
    tIncomingFrameHanlder() : tMessageReciever() {}

    virtual void onMessage(uint8_t type, uint16_t data, void *pData) { handleCommonMessages(type, data, pData); }

protected:
    uint8_t handleCommonMessages(uint8_t type, uint16_t data, void *pData);
};

#endif // CONFIG_TLE8457_COMM_LIB
