#pragma once


#include "../../../global.h"
#if CONFIG_TLE8457_COMM_LIB

#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"

#define RECIEVE_NUMBER_OF_RETRANS_TABLE 10  // number of pairs senderId-seq kept to recognize retransmissions

class CommRecieverProcess : public  Process
{
 public:
  static CommRecieverProcess *Instance;
  CommRecieverProcess(Scheduler &manager, uint8_t SelfDevId);

  void clearSelfFrameMark() { mSelfFrameMark = false; }
  bool getSelfFrameMark() { return mSelfFrameMark; }

  void serialEvent();
  virtual void service();

  private:
    tCommunicationFrame mFrame;
    static const uint8_t STATE_IDLE = 0;          // completely stopped, RetransTable clean
    static const uint8_t STATE_NEW_TRIGGER = 1;   // new data arrived when state was idle
    static const uint8_t STATE_WAIT_FOR_DATA = 2; // waiting for a frame
    static const uint8_t STATE_WAIT_FOR_IDLE = 3; // waiting for idle, keeping retrans table

    uint8_t mState;

    typedef struct
    {
      uint8_t SenderID;
      uint8_t Seq;
    } tRetransTable;

    void SetState(uint8_t State);
    void CleanIncomingBuffer();
    void ProcessFrame();
    uint8_t mSelfDevId;
    tRetransTable mRetransTable[RECIEVE_NUMBER_OF_RETRANS_TABLE];
    uint8_t mRetransTableHead;
    bool mSelfFrameMark;
};

#endif // CONFIG_TLE8457_COMM_LIB
