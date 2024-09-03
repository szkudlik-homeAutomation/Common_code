#include "../../../global.h"

#if CONFIG_TLE8457_COMM_LIB

#include "TLE8457_serial_lib.h"
#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../../lib/AceCRC/src/AceCRC.h"

#include "CommSender.h"
#include "../lib/Random.h"
#include "CommReciever.h"

#define htons(x) ( ((x)<< 8 & 0xFF00) | \
                   ((x)>> 8 & 0x00FF) )

using namespace ace_crc::crc16ccitt_nibble;


CommSenderProcess *CommSenderProcess::Instance = NULL;

CommSenderProcess::CommSenderProcess() :
    Process(MEDIUM_PRIORITY,SERVICE_CONSTANTLY),
    mQueue(CONFIG_OUTPUT_QUEUE_SIZE),
    isSending(false)
    {
	  Instance = this;
      mFrame.SenderDevId = 0;
      mFrame.Seq = 0;
    };

void CommSenderProcess::Enqueue(uint8_t DstDevId, uint8_t MessageType, uint8_t DataSize, void *pData)
{
	if (mFrame.SenderDevId == 0) {
		DEBUG_PRINTLN_2("Sender not configured");
		return;
	}

  tQueueItem qItem;
  if (DataSize > sizeof(qItem.Data)) return;
  qItem.DstDevId = DstDevId;
  qItem.MessageType = MessageType;
  qItem.DataSize = DataSize;
  uint8_t * pDataTable = pData;
  for(uint8_t i = 0; i < DataSize; i++) qItem.Data[i] = pDataTable[i];

  mQueue.enqueue(qItem);

  if (! isEnabled())
  {
    setPeriod(SERVICE_CONSTANTLY);
    setIterations(RUNTIME_FOREVER);
    enable();
  }
}

void CommSenderProcess::service()
{
  if (isSending)
  {
    // wait till the frame has been physically sent
    COMM_SERIAL.flush();
    if (true == CommRecieverProcess::Instance->getSelfFrameMark())
    {
      // frame sent properly, decrease retranmissions
      mRetransLeft--;
    }
    else
    {
       // a collision detected
       mCollisionRetransLeft--;
    }
    CommRecieverProcess::Instance->clearSelfFrameMark();
  if ((0 == mRetransLeft) || (0 == mCollisionRetransLeft))
      isSending = false;
  }

    // isSending may have changed
  if (! isSending)
  {
    // prepare another frame
    if (DequeueFrame())
    {
      mCollisionRetransLeft = CONFIG_MAX_NUM_OF_RETRANSMISSIONS;
      mRetransLeft = CONFIG_NUM_OF_RETRANSMISSIONS;
      CommRecieverProcess::Instance->clearSelfFrameMark();
      isSending = true;
    }
    else
    {
      disable();
      return;
    }
  }

    // isSending may have changed... again
  if (isSending)
  {
    COMM_SERIAL.write((char*)&mFrame,sizeof(mFrame));
    uint16_t NextTimeout = (mRandom.Get() % CONFIG_MAX_TRANSMIT_DELAY) + frameTransmissionTime;
    setPeriod(NextTimeout);
  }
}

bool CommSenderProcess::DequeueFrame()
{
    if (mQueue.isEmpty()) return false;
    DEBUG_PRINTLN_2("---->prepare frame from the queue");
    tQueueItem Item = mQueue.dequeue();
    // prepare a frame
    mFrame.DstDevId = Item.DstDevId;
    mFrame.MessageType = Item.MessageType;

    for (uint8_t i = 0; i < CONFIG_COMMUNICATION_PAYLOAD_DATA_SIZE; i++) mFrame.Data[i] = 0;
    if (Item.DataSize) memcpy(mFrame.Data,Item.Data,Item.DataSize);

    // set seq number
    mFrame.Seq++;

    // calculate CRC
    crc_t crc = crc_calculate(&mFrame, sizeof(mFrame) - sizeof(mFrame.crc));
    mFrame.crc = htons(crc);

    // trigger
    return true;
}
#endif // CONFIG_TLE8457_COMM_LIB
