/*
 * tMessage.cpp
 *
 *  Created on: 8 mar 2023
 *      Author: szkud
 */

#include "tMessages.h"
#include "tMessageReciever.h"


void tMessages::VersionResponseHandler(uint8_t SenderID, uint8_t Major, uint8_t Minor, uint8_t Patch)
{
	tVersionResponse  VersionResponse;

	VersionResponse.SenderID = SenderID;
	VersionResponse.Major = Major;
	VersionResponse.Minor = Minor;
	VersionResponse.Patch = Patch;
	VersionResponseHandler(&VersionResponse);
}

void tMessages::VersionResponseHandler(struct tVersionResponse *pVersionResponse)
{
	LOG_PRINT("FW Version for device ");
	LOG(print(pVersionResponse->SenderID,HEX));
	LOG_PRINT("=");
	LOG(print(pVersionResponse->Major,DEC));
	LOG_PRINT(".");
	LOG(print(pVersionResponse->Minor,DEC));
	LOG_PRINT(".");
	LOG(println(pVersionResponse->Patch,DEC));

	tMessageReciever::Dispatch(tMessages::MessageType_ExternalEvent,ExternalEvent_VersionResponse,pVersionResponse);
}

#if CONFIG_OUTPUT_PROCESS

static void tMessages::OutputStateResponseHandler(uint8_t SenderID, uint8_t OutputID, uint8_t PowerState, uint16_t  TimerValue, uint16_t DefaultTimer)
{
	tOutputStateResponse OutputStateResponse;
	OutputStateResponse.SenderID = SenderID;
	OutputStateResponse.OutputID = OutputID;
	OutputStateResponse.PowerState = PowerState;
	OutputStateResponse.TimerValue = TimerValue;
	OutputStateResponse.DefaultTimer = DefaultTimer;
	OutputStateResponseHandler(&OutputStateResponse);
}

static void tMessages::OutputStateResponseHandler(struct tOutputStateResponse* pOutputStateResponse)
{
	LOG_PRINT("PowerState for device ");
	LOG(print(pOutputStateResponse->SenderID,HEX));
	LOG_PRINT(" output ID ");
	LOG(print(pOutputStateResponse->OutputID,DEC));
	LOG_PRINT("=");
	LOG(print(pOutputStateResponse->PowerState,DEC));
	LOG_PRINT(" with timers = ");
	LOG(print(pOutputStateResponse->TimerValue,DEC));
    LOG_PRINT(" default timer = ");
    LOG(println(pOutputStateResponse->DefaultTimer,DEC));

    tMessageReciever::Dispatch(tMessages::MessageType_ExternalEvent,ExternalEvent_OutputStateResponse,pOutputStateResponse);
}

void tMessages::OverviewStateResponseHandler(uint8_t SenderID, uint8_t PowerState, uint8_t  TimerState)
{
	tOverviewStateResponse OverviewStateResponse;

	LOG_PRINT("PowerStateBitmap for device ");
	LOG(print(SenderID,HEX));
	LOG_PRINT("=");
	LOG(print(PowerState,BIN));
	LOG_PRINT(" with timers map=");
	LOG(println(TimerState,BIN));

	OverviewStateResponse.SenderID = SenderID;
	OverviewStateResponse.PowerState = PowerState;
	OverviewStateResponse.TimerState = TimerState;

    tMessageReciever::Dispatch(tMessages::MessageType_ExternalEvent,ExternalEvent_OverviewStateResponse,&OverviewStateResponse);
}

#endif // CONFIG_OUTPUT_PROCESS
