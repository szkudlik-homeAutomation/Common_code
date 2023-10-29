#include "../../global.h"
#if CONFIG_OUTPUT_PROCESS

#include "OutputProcess.h"

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
