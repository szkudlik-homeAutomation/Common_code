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

	/*
	 * an event from external - i.e. TCP or Serial communication
	 * list of events below
	 */
	static const uint8_t MessageType_ExternalEvent = 1;
	  static const uint8_t ExternalEvent_VersionResponse = 0;
	  static const uint8_t ExternalEvent_OutputStateResponse = 1;

	static const uint8_t MessageType_SensorEvent   = 2;

	// NOTE - app specific event types start from 16




	  typedef struct tVersionResponse
	  {
		  uint8_t SenderID;
		  uint8_t Major;
		  uint8_t Minor;
		  uint8_t Patch;
	  };
	  /*
	   * log and dispatch tMessages::MessageType_ExternalEvent::ExternalEvent_VersionResponse response through tMessageReciever
	   */
	  static void VersionResponseHandler(uint8_t SenderID, uint8_t Major, uint8_t Minor, uint8_t Patch);
	  static void VersionResponseHandler(struct tVersionResponse *pVersionResponse);

#if CONFIG_OUTPUT_PROCESS
	  /*
	   * log and dispatch tMessages::MessageType_ExternalEvent::ExternalEvent_OutputStateResponse state response through tMessageReciever
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

#if CONFIG_NODE_SCAN_TASK
	    static void NodeScanResponse(uint32_t ActiveNodesMap);
#endif CONFIG_NODE_SCAN_TASK

};
