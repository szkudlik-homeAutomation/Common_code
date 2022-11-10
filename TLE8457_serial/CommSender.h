#ifndef COMM_SENDER
#define COMM_SENDER

#include "../../../global.h"

#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include <ArduinoQueue.h>

#include "Random.h"

class CommSenderProcess : public Process
{
  public:
  CommSenderProcess(Scheduler &manager, uint8_t RandomSeed, uint8_t SenderDevId);

  void Enqueue(uint8_t DstDevId, uint8_t MessageType, uint8_t DataSize, void *pData);

  virtual void service();

private:
  typedef struct
  {
    uint8_t DstDevId;
    uint8_t MessageType;
    uint8_t DataSize;
    uint8_t Data[COMMUNICATION_PAYLOAD_DATA_SIZE];
  } tQueueItem;

  ArduinoQueue<tQueueItem> mQueue;

  tCommunicationFrame mFrame;
  uint8_t mDataSize;
  Random mRandom;
  uint8_t mCollisionRetransLeft;
  uint8_t mRetransLeft;
  bool isSending;

  bool DequeueFrame();
};




extern CommSenderProcess CommSender;


#endif
