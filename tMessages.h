#pragma once
/*
 * a handler of messages to be used in all applications
 * uses tMessageReciever to dispatch messages
 */
#include "../../global.h"

class tMessages {
public:
	  static const uint8_t frameRecieved_VersionResponse = 0;
	  static const uint8_t frameRecieved_OutputStateResponse = 1;

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

#if CONFIG_OUTPUT_PROCESS
	  /*
	   * log and dispatch MessageType_frameRecieved::frameRecieved_OutputStateResponseouput state response through tMessageReciever
	   */
	  typedef struct tOutputStateResponse
	  {
		  uint8_t SenderID;
		  uint8_t OutputID;
		  uint8_t PowerState;
		  uint16_t TimerValue;
		  uint16_t DefaultTimer;
	  };
	  static void OutputStateResponseHandler(uint8_t SenderID, uint8_t OutputID, uint8_t PowerState, uint16_t  TimerValue, uint16_t DefaultTimer);
	  static void OutputStateResponseHandler(struct tOutputStateResponse* pOutputStateResponse);

	  static void OverviewStateResponseHandler(uint8_t SenderID, uint8_t PowerState, uint8_t  TimerState);

#endif CONFIG_OUTPUT_PROCESS

};
