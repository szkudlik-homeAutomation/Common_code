#pragma once


#include "../../../global.h"

#if CONFIG_TLE8457_COMM_LIB

#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../../lib/ArduinoQueue/ArduinoQueue.h"

#include "../lib/Random.h"

class CommSenderProcess : public Process
{
  public:
  static CommSenderProcess *Instance;

  // static const calculated in compilation time
#ifdef FRAME_TRANSMISSION_TIME
  static const uint32_t frameTransmissionTime = FRAME_TRANSMISSION_TIME;
#else
  static const uint32_t frameTransmissionTime = ((( (uint32_t)11000 * (uint32_t)sizeof(tCommunicationFrame) ) / (uint32_t)CONFIG_TRANSMISSION_SPEED ) + 1);
#endif

  CommSenderProcess();

  void SetSelfDevId(uint8_t SenderDevId) {
	  mFrame.SenderDevId = SenderDevId;
	  mRandom.SetSeed(SenderDevId);
	  mSenderId = SenderDevId;
  }

  uint8_t GetSelfDevId() {
		return mFrame.SenderDevId;
	}

  void Enqueue(uint8_t DstDevId, uint8_t MessageType, uint8_t DataSize, void *pData);

  virtual void service();

private:
  typedef struct
  {
    uint8_t DstDevId;
    uint8_t MessageType;
    uint8_t DataSize;
    uint8_t Data[CONFIG_COMMUNICATION_PAYLOAD_DATA_SIZE];
  } tQueueItem;

  ArduinoQueue<tQueueItem> mQueue;

  tCommunicationFrame mFrame;
  uint8_t mDataSize;
  Random mRandom;
  uint8_t mCollisionRetransLeft;
  uint8_t mRetransLeft;
  bool isSending;
  uint8_t mSenderId;

  bool DequeueFrame();
};


#endif // CONFIG_TLE8457_COMM_LIB
