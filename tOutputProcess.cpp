#include "../../global.h"
#if CONFIG_OUTPUT_PROCESS

#include "tOutputProcess.h"
#include "TLE8457_serial/tOutgoingFrames.h"
#include "TLE8457_serial/TLE8457_serial_lib.h"
#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"

#if CONFIG_OUTPUT_PROCESS_INSTANCE
class tOutputProcess_default : public tOutputProcess
{
	public:
    tOutputProcess_default() : tOutputProcess() {}
    virtual void setup()
    {
#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 0
    	Output[0].SetPin(CONFIG_OUTPUT_PROCESS_PIN0,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW0
				0
#else
				1
#endif
        );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 1
    	Output[1].SetPin(CONFIG_OUTPUT_PROCESS_PIN1,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW1
				0
#else
				1
#endif
        );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 2
    	Output[2].SetPin(CONFIG_OUTPUT_PROCESS_PIN2,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW2
				0
#else
				1
#endif
		);
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 3
    	Output[3].SetPin(CONFIG_OUTPUT_PROCESS_PIN3,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW3
				0
#else
				1
#endif
		);
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 4
    	Output[4].SetPin(CONFIG_OUTPUT_PROCESS_PIN4,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW4
				0
#else
				1
#endif
		);
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 5
    	Output[5].SetPin(CONFIG_OUTPUT_PROCESS_PIN5,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW5
				0
#else
				1
#endif
		);
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 6
    	Output[6].SetPin(CONFIG_OUTPUT_PROCESS_PIN6,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW6
    			0
#else
				1
#endif
  );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 7
    	Output[7].SetPin(CONFIG_OUTPUT_PROCESS_PIN7,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW7
    			0
#else
				1
#endif
  );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 8
    	Output[8].SetPin(CONFIG_OUTPUT_PROCESS_PIN8,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW8
    			0
#else
				1
#endif
  );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 9
    	Output[9].SetPin(CONFIG_OUTPUT_PROCESS_PIN9,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW9
    			0
#else
				1
#endif
  );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 10
    	Output[10].SetPin(CONFIG_OUTPUT_PROCESS_PIN10,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW10
    			0
#else
				1
#endif
  );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 11
    	Output[11].SetPin(CONFIG_OUTPUT_PROCESS_PIN11,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW11
    			0
#else
				1
#endif
  );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 12
    	Output[12].SetPin(CONFIG_OUTPUT_PROCESS_PIN12,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW12
    			0
#else
				1
#endif
  );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 13
    	Output[13].SetPin(CONFIG_OUTPUT_PROCESS_PIN13,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW13
    			0
#else
				1
#endif
  );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 14
    	Output[14].SetPin(CONFIG_OUTPUT_PROCESS_PIN14,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW14
    			0
#else
				1
#endif
  );
#endif

#if CONFIG_OUTPUT_PROCESS_NUM_OF_PINS > 15
    	Output[15].SetPin(CONFIG_OUTPUT_PROCESS_PIN15,
#if CONFIG_OUTPUT_PROCESS_PIN_ACTIVE_LOW15
    			0
#else
				1
#endif
  );
#endif
    }
};

tOutputProcess_default OutputProcess;
#endif

static tOutputProcess *tOutputProcess::Instance;

void tOutputProcess::service()
{
	for (uint8_t i = 0; i < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS; i++)
	{
    Output[i].Tick();
	}
}


uint8_t  tOutputProcess::GetOutputStateMap()
{
  uint8_t Map = 0;
  for (uint8_t i = 0; i < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS; i++)
  {
    if (Output[i].GetState())
      Map |= 1 << i;
  }

  return Map;
}

uint8_t  tOutputProcess::GetOutputTimersStateMap()
{
  uint8_t Map = 0;
  for (uint8_t i = 0; i < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS; i++)
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
            if (Message->OutputID < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS)
            {
                tOutgoingFrames::SendMsgOutputStateResponse(pFrame->SenderDevId,Message->OutputID,GetOutputState(Message->OutputID), GetOutputTimer(Message->OutputID),0);
            }
        }
        break;

    case MESSAGE_TYPE_SET_OUTPUT:
        {
            tMessageTypeSetOutput* Message = (tMessageTypeSetOutput*)(pFrame->Data);
            if (Message->OutId < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS)
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
