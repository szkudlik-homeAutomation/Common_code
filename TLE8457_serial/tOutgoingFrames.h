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
    // frames that always should be supported
    static bool SendMsgVersionRequest(uint8_t RecieverID);
    static bool SendMsgVersionResponse(uint8_t RecieverID, uint8_t Major, uint8_t Minor, uint8_t Patch);
    static bool SendMsgReset(uint8_t RecieverID);
    static bool SendMsgStatus(uint8_t RecieverID, uint8_t Status);

#if CONFIG_OUTPUT_PROCESS
    static bool SendMsgOverviewStateRequest(uint8_t RecieverID);
    static bool SendMsgOverviewStateResponse(uint8_t RecieverID, uint8_t  PowerState, uint8_t  TimerState);
    static bool SendMsgOutputStateRequest(uint8_t RecieverID, uint8_t  OutputID);
    static bool SendMsgOutputStateResponse(uint8_t RecieverID, uint8_t  OutputID, uint8_t  PowerState, uint16_t TimerValue, uint16_t DefaultTimer);
    static bool SendMsgSetOutput(uint8_t RecieverID, uint8_t  OutId, uint8_t  State, uint16_t Timer);
#endif // CONFIG_OUTPUT_PROCESS
#if CONFIG_SENSORS
    static bool SendSensorConfigure(uint8_t RecieverID, uint8_t SensorID, uint8_t seq, bool LastSegment, void *pPayload, uint8_t payloadSize, uint16_t MeasurementPeriod);
#endif //CONFIG_SENSORS
};

#endif /* CONFIG_TLE8457_COMM_LIB */
