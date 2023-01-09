#pragma once


#include "../../global.h"
#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"

/*
 * requires NUM_OF_OUTPUTS defined
 * create a new class and override setup. Init all Output[] objets
 *
 *  Output[OUT_ID_xxx].SetPin(OUT_PIN_xxx,OUT_PIN_POLARITY_xxx);
 */



#define OUTPUT_SERVICE_TIME 1000  // 1 second

class tOutput
{
   static const uint8_t PIN_NOT_ASSIGNED = 255;
public:
  tOutput() :mPin(PIN_NOT_ASSIGNED) {}

  void SetPin(uint8_t pin, uint8_t Polarity) { mPin = pin; mPolarity = Polarity; pinMode(mPin, OUTPUT); SetState(0); }

  void Set(uint8_t State, uint16_t Timer, bool timerLongerOnly)
  {
     if (timerLongerOnly && mState && (mTimer != 0) && (mTimer > Timer))
        // if we don't want to make the timer shorter
        //    and the timer is active (mState, mTimer != 0)
        //    and the new time is shorter than current time
        //        ==> don't change the timer value
           Timer = mTimer;

     if (State) mTimer = Timer; else mTimer = 0;
     SetState(State);
  }
  void Toggle(uint16_t Timer)
  {
    SetState(! mState);
    if (mState) mTimer = Timer; else mTimer = 0;
  }
  void Tick();

  uint16_t GetTimer() const { return (mTimer); }
  uint16_t GetState() const { return (mState); }

private:

  void SetState(uint8_t State);
  uint8_t mPin;
  uint16_t mTimer;
  uint8_t mState   : 1,
         mPolarity : 1,
         Reserved  : 6;
};

class tOutputProcess : public  Process
{
  public:
  tOutputProcess(Scheduler &manager) :
    Process(manager,LOW_PRIORITY,OUTPUT_SERVICE_TIME) {}

  static const bool TimerLongerOnly = true;
  static const bool ForceTimer = false;


  void SetOutput(uint8_t outputId, uint8_t State)
  {
     SetOutput(outputId, State, 0, false);
  }

  void SetOutput(uint8_t outputId, uint8_t State, uint16_t timer)
  {
     SetOutput(outputId, State, timer, false);
  }

  void SetOutput(uint8_t outputId, uint8_t State, uint16_t timer, bool timerLongerOnly)
  {
    if (outputId < NUM_OF_OUTPUTS) Output[outputId].Set(State,timer,timerLongerOnly);
  }

  void ToggleOutput(uint8_t outputId, uint16_t timer)
  {
    if (outputId < NUM_OF_OUTPUTS) Output[outputId].Toggle(timer);
  }

  uint8_t  GetOutputStateMap();
  uint8_t  GetOutputTimersStateMap();
  uint8_t  GetOutputState(uint8_t outputId)
  {
    if (outputId < NUM_OF_OUTPUTS)
      return (Output[outputId].GetState());
    else return 0;
  }

  uint16_t GetOutputTimer(uint8_t outputId)
  {
    if (outputId < NUM_OF_OUTPUTS)
      return (Output[outputId].GetTimer());
    else return 0;
  }


  virtual void service();
protected:
  tOutput Output[NUM_OF_OUTPUTS];
};
