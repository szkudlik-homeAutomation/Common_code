#pragma once
/*
 * a handler of messages to be used in all applications
 * uses tMessageReciever to dispatch messages
 */
#include "../../global.h"

class tMessages {
public:
	  static const uint8_t frameRecieved_VersionResponse = 0;		// app specific codes should be >= 128
	  typedef struct tVersionResponse
	  {
		  uint8_t SenderID;
		  uint8_t Major;
		  uint8_t Minor;
		  uint8_t Patch;
	  };
	  /*
	   * log and dispatch MessageType_frameRecieved::frameRecieved_VersionResponse response through tMessageReciever
	   */
	  static void VersionResponseHandler(uint8_t SenderID, uint8_t Major, uint8_t Minor, uint8_t Patch);
	  static void VersionResponseHandler(struct tVersionResponse *pVersionResponse);
};
