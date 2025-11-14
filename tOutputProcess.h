#pragma once


/*
 Arduino nano pin mapping

 ATmega328P Pin (Physical)	Port Name	Arduino Pin Number
2	PD0	0
3	PD1	1
4	PD2	2
5	PD3	3
6	PD4	4
11	PD5	5
12	PD6	6
13	PD7	7
14	PB0	8
15	PB1	9
16	PB2	10
17	PB3	11
18	PB4	12
19	PB5	13
23	PC0	14 (A0)
24	PC1	15 (A1)
25	PC2	16 (A2)
26	PC3	17 (A3)
27	PC4	18 (A4)
28	PC5	19 (A5)
 */

#include "../../global.h"
#if CONFIG_OUTPUT_PROCESS

#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "tMessageReciever.h"

#define OUTPUT_SERVICE_TIME 1000  // 1 second

#define DEFAULT_TIMER 0xFFFF

class tOutput
{
public:
  static const uint8_t PIN_NOT_ASSIGNED = 255;
  tOutput() :mPin(PIN_NOT_ASSIGNED) {}

  // polarity 1 means active high, 0 means active low
  void SetPin(uint8_t pin, uint8_t Polarity);

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
         mPolarity : 1,	// 0 - active low, 1 - active high
         Reserved  : 6;
};

class tOutputProcess : public  Process, public tMessageReciever
{
public:
	static tOutputProcess *Instance;

  tOutputProcess() : Process(LOW_PRIORITY,OUTPUT_SERVICE_TIME),
    tMessageReciever()
    {
      Instance = this;
#if CONFIG_TLE8457_COMM_LIB
      RegisterMessageType(MessageType_SerialFrameRecieved);
#endif CONFIG_TLE8457_COMM_LIB
    }

  static const bool TimerLongerOnly = true;
  static const bool ForceTimer = false;


  uint8_t SetOutput(uint8_t outputId, uint8_t State)
  {
     return SetOutput(outputId, State, 0, false);
  }

  uint8_t SetOutput(uint8_t outputId, uint8_t State, uint16_t timer)
  {
     return SetOutput(outputId, State, timer, false);
  }

  uint8_t SetOutput(uint8_t outputId, uint8_t State, uint16_t timer, bool timerLongerOnly)
  {
    if (outputId < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS)
    {
    	Output[outputId].Set(State,timer,timerLongerOnly);
    	return STATUS_SUCCESS;
    }
    return STATUS_UNSUPPORTED;
  }

  uint8_t ToggleOutput(uint8_t outputId, uint16_t timer)
  {
    if (outputId < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS)
    {
    	Output[outputId].Toggle(timer);
    	return STATUS_SUCCESS;
    }
    return STATUS_UNSUPPORTED;
  }

  uint8_t  GetOutputStateMap();
  uint8_t  GetOutputTimersStateMap();
  uint8_t  GetOutputState(uint8_t outputId)
  {
    if (outputId < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS)
      return (Output[outputId].GetState());
    else return 0;
  }

  uint16_t GetOutputTimer(uint8_t outputId)
  {
    if (outputId < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS)
      return (Output[outputId].GetTimer());
    else return 0;
  }


  virtual void service();
protected:
  tOutput Output[CONFIG_OUTPUT_PROCESS_NUM_OF_PINS];

protected:
    virtual void onMessage(uint8_t type, uint16_t data, void *pData);
};

#endif // CONFIG_OUTPUT_PROCESS
