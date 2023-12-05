#include "../../global.h"
#if CONFIG_OUTPUT_PROCESS

#include "tOutputProcess.h"
#include "TLE8457_serial/tOutgoingFrames.h"
#include "TLE8457_serial/TLE8457_serial_lib.h"

static tOutputProcess *tOutputProcess::instance;

void tOutputProcess::service()
{
	for (uint8_t i = 0; i < NUM_OF_OUTPUTS; i++)
	{
    Output[i].Tick();
	}
}


uint8_t  tOutputProcess::GetOutputStateMap()
{
  uint8_t Map = 0;
  for (uint8_t i = 0; i < NUM_OF_OUTPUTS; i++)
  {
    if (Output[i].GetState())
      Map |= 1 << i;
  }

  return Map;
}

uint8_t  tOutputProcess::GetOutputTimersStateMap()
{
  uint8_t Map = 0;
  for (uint8_t i = 0; i < NUM_OF_OUTPUTS; i++)
  {
    if (Output[i].GetTimer())
      Map |= 1 << i;
  }

  return Map;
}

void tOutput::Tick()
{
  if (mTimer)
  {
    mTimer--;
    if (0 == mTimer) SetState(0);
  }
}

void tOutputProcess::onMessage(uint8_t type, uint16_t data, void *pData)
{
#if CONFIG_TLE8457_COMM_LIB
    if (type != MessageType_SerialFrameRecieved)
        return;

    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;

    switch (data)   // messageType
    {

    case MESSAGE_TYPE_OVERVIEW_STATE_REQUEST:
        tOutgoingFrames::SendMsgOverviewStateResponse(pFrame->SenderDevId,GetOutputStateMap(),GetOutputTimersStateMap());
        break;

    case MESSAGE_TYPE_OUTPUT_STATE_REQUEST:
        {
            uint16_t DefTimer;
            tMessageTypeOutputStateRequest* Message = (tMessageTypeOutputStateRequest*)(pFrame->Data);
            if (Message->OutputID < NUM_OF_OUTPUTS)
            {
                tOutgoingFrames::SendMsgOutputStateResponse(pFrame->SenderDevId,Message->OutputID,GetOutputState(Message->OutputID), GetOutputTimer(Message->OutputID),0);
            }
        }
        break;

    case MESSAGE_TYPE_SET_OUTPUT:
        {
            tMessageTypeSetOutput* Message = (tMessageTypeSetOutput*)(pFrame->Data);
            if (Message->OutId < NUM_OF_OUTPUTS)
            {
               uint16_t Timer = Message->Timer;
               SetOutput(Message->OutId,Message->State,Timer,ForceTimer);
            }
        }
        break;
    }
#endif // CONFIG_TLE8457_COMM_LIB
}

void tOutput::SetState(uint8_t State)
{
  DEBUG_PRINT_2("========================>Setting output:");
  DEBUG_2(print(mPin,DEC));
  DEBUG_PRINT_2(" to state ");
  if (State) DEBUG_PRINTLN_2("ACTIVE");
        else DEBUG_PRINTLN_2("INACTICVE");

  mState = State;
  if (PIN_NOT_ASSIGNED != mPin)
  {
     if (mPolarity)
     {
        digitalWrite(mPin,(mState ? HIGH : LOW)); //reversed
     }
     else
     {
        digitalWrite(mPin,(mState ? LOW  : HIGH)); // normal
     }
  }

}
#endif // CONFIG_OUTPUT_PROCESS
