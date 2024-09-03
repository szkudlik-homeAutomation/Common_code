#include "../../../global.h"

#if CONFIG_TLE8457_COMM_LIB

#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../../lib/AceCRC/src/AceCRC.h"
#include "../tMessageReciever.h"

#include "TLE8457_serial_lib.h"
#include "CommReciever.h"
#include "CommSender.h"

using namespace ace_crc::crc16ccitt_nibble;

CommRecieverProcess *CommRecieverProcess::Instance = NULL;

void COMM_SERIAL_EVENT() {
  CommRecieverProcess::SerialEventCallback();
}

CommRecieverProcess::CommRecieverProcess()
   : Process(LOW_PRIORITY,CommSenderProcess::frameTransmissionTime),
     mRetransTableHead(0),
     mSelfDevId(0)
{
  Instance = this;
  SetState(STATE_NOT_CONFIGURED);
}

void CommRecieverProcess::CleanIncomingBuffer()
{
  DEBUG_PRINTLN_1("Clean buffer");
  while (COMM_SERIAL.available()) COMM_SERIAL.read();
}

void CommRecieverProcess::SetState(uint8_t State)
{
  mState = State;
  switch (mState)
  {
    case STATE_IDLE:
        for (uint8_t i = 0; i < RECIEVE_NUMBER_OF_RETRANS_TABLE; i++)
          mRetransTable[i].SenderID = DEVICE_ID_BROADCAST;  // BROADCAST means an empty slot
        setIterations(0);
        DEBUG_PRINTLN_1("------> State Idle");
        break;

    case STATE_NEW_TRIGGER:
       setIterations(3);
       setPeriod(SERVICE_CONSTANTLY);
       enable();
       DEBUG_PRINTLN_1("------> State NEW_TRIGGER");
       break;

    case STATE_WAIT_FOR_DATA:
      setPeriod(CommSenderProcess::frameTransmissionTime);
      DEBUG_PRINTLN_1("------> State WAIT_FOR_DATA");
      break;

    case STATE_WAIT_FOR_IDLE:
      setPeriod(CommSenderProcess::frameTransmissionTime * 100);
      DEBUG_PRINTLN_1("------> State IDLE_WAIT");
      break;
  }
}

void CommRecieverProcess::serialEvent()
{
  if ((mState == STATE_IDLE) ||
      (mState == STATE_WAIT_FOR_IDLE))
  {
    SetState(STATE_NEW_TRIGGER);
  }
}

void CommRecieverProcess::service()
{
  DEBUG_PRINTLN_1("SERVICE enter");

  switch (mState)
  {
	case STATE_NOT_CONFIGURED:
		// do nothing
		break;

    case STATE_NEW_TRIGGER:
      if (COMM_SERIAL.available() < sizeof(mFrame))
      {
        SetState(STATE_WAIT_FOR_DATA);
        // next iteration
        break;
      }

      // else - proceed to next step,
    case STATE_WAIT_FOR_DATA:
      if (COMM_SERIAL.available() < sizeof(mFrame))
      {
        // timeout - drop
        DEBUG_PRINTLN_1("Timeout - drop data");
        CleanIncomingBuffer();
      }
      else
      {
        // frame is ready
        DEBUG_PRINTLN_1("Reading frame");
        COMM_SERIAL.readBytes((char*)&mFrame,sizeof(mFrame));
        ProcessFrame();
      }
      SetState(STATE_WAIT_FOR_IDLE);
      break;

    case STATE_WAIT_FOR_IDLE:
      SetState(STATE_IDLE);
      break;
  }

  DEBUG_PRINTLN_1("SERVICE leaving");
}

void CommRecieverProcess::ProcessFrame()
{
  // check CRC
  crc_t crc = crc_calculate(&mFrame, sizeof(mFrame));
  if (0 != crc)
  {
  // drop frame - CRC is not valid
     DEBUG_PRINTLN_2("bad CRC - reject");
     return;
  }


  #if CONFIG_LOGLEVEL_1
    // printout frame
    CONFIG_LOGGER_SERIAL.print(F("SenderDevId = 0x"));
    CONFIG_LOGGER_SERIAL.print(mFrame.SenderDevId,HEX);
    CONFIG_LOGGER_SERIAL.print(F(" DstDevId = 0x"));
    CONFIG_LOGGER_SERIAL.print(mFrame.DstDevId,HEX);
    CONFIG_LOGGER_SERIAL.print(F(" Seq = 0x"));
    CONFIG_LOGGER_SERIAL.print(mFrame.Seq,HEX);
    CONFIG_LOGGER_SERIAL.print(F(" MessageType = 0x"));
    CONFIG_LOGGER_SERIAL.print(mFrame.MessageType,HEX);
    CONFIG_LOGGER_SERIAL.print(F(" Data = 0x"));
    for (uint8_t i = 0; i < CONFIG_COMMUNICATION_PAYLOAD_DATA_SIZE; i++)
    {
      CONFIG_LOGGER_SERIAL.print(mFrame.Data[i],HEX);
    }
    CONFIG_LOGGER_SERIAL.println();
  #endif

  // are we the sender?
  if (mFrame.SenderDevId ==  mSelfDevId)
  {
    // this is a frame sent by us, mark that it has been properly recieved and drop the frame
     DEBUG_PRINTLN_2("SELF SENT FRAME RECIEVED");
     mSelfFrameMark = true;
  }

  // are we the reciever?
  if (!( (mFrame.DstDevId == DEVICE_ID_BROADCAST) || (mFrame.DstDevId == mSelfDevId)))
  {
     // drop
     DEBUG_PRINTLN_2("Dst does not match - reject");
     return;
  }

  // check retransmissions
  for (uint8_t i = 0; i < RECIEVE_NUMBER_OF_RETRANS_TABLE; i++)
  {
     if (mRetransTable[i].SenderID == DEVICE_ID_BROADCAST) continue;  // empty table slot

     if ((mRetransTable[i].SenderID == mFrame.SenderDevId) && (mRetransTable[i].Seq == mFrame.Seq))
     {
         // a retransmission - drop
         DEBUG_PRINTLN_2("Retransmission - drop");
         return;
     }
  }

   // this is not an retransmission
   mRetransTable[mRetransTableHead].SenderID = mFrame.SenderDevId;
   mRetransTable[mRetransTableHead].Seq = mFrame.Seq;
   mRetransTableHead++;
   mRetransTableHead %= RECIEVE_NUMBER_OF_RETRANS_TABLE;

   //  dispatch message
   tMessageReciever::Dispatch(MessageType_SerialFrameRecieved, mFrame.MessageType, &mFrame);
}

#endif // CONFIG_TLE8457_COMM_LIB
