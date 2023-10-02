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

	tMessageReciever::Dispatch(tMessageReciever::MessageType_frameRecieved,frameRecieved_VersionResponse,pVersionResponse);
}
