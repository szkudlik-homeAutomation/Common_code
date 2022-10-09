#include "../../global.h"
#include "OutputProcess.h"

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
  #ifdef DEBUG_2
  DEBUG_SERIAL.print(F("========================>Setting output:"));
  DEBUG_SERIAL.print(mPin,DEC);
  DEBUG_SERIAL.print(F(" to state "));
  if (State) DEBUG_SERIAL.println(F("ACTIVE"));
        else DEBUG_SERIAL.println(F("INACTICVE"));
  #endif
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
