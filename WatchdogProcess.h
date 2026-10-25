#pragma once

#include "../../global.h"

#if CONFIG_WATCHDOG
#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../lib/Watchdog/src/Watchdog.h"

/**
 * a base class for watchdog item that need to be triggered periodically
 *
 * if any of the items is not triggered at least once in required time the system must be restarted
 */
class tWatchdogItem
{
public:
   tWatchdogItem(uint16_t NumOfSeconds)
      : mNumOfSeconds(NumOfSeconds)
   {
      pNext = pFirst;
      pFirst = this;
      Reset();
   }

   void Reset() {mCurrentTimeoutCnt = mNumOfSeconds;}

   virtual void doRecovery() {}

private:
   bool Tick(); friend class tWatchdogProcess;

   uint16_t mNumOfSeconds;
   uint16_t mCurrentTimeoutCnt;

   static tWatchdogItem *pFirst;
   tWatchdogItem *pNext;
};


class tWatchdogProcess : public  Process
{
  public:
  tWatchdogProcess() : Process(WATCHDOG_PRIORITY, SERVICE_SECONDLY, RUNTIME_FOREVER) 
  {
      #if CONFIG_WATCHDOG_GPIO_RESET
      mGpioTimeoutCnt = 0;
      pinMode(CONFIG_WATCHDOG_GPIO_RESET_PIN, INPUT); // set to input, disable internal pull-up 
      digitalWrite(CONFIG_WATCHDOG_GPIO_RESET_PIN, LOW);
      #endif // CONFIG_WATCHDOG_GPIO_RESET

      // reset watchdog here to avoid reset during boot, before all items are created and registered
      watchdog.reset();
      // enable watchdog with 8s timeout
      watchdog.enable(Watchdog::TIMEOUT_8S);
  }
  virtual void service();

  // perform GPIO reset (if enabled) and than stop interrupts and wait for HW watchdog to reset the system
  void resetSystem();

#if CONFIG_WATCHDOG_GPIO_RESET
  void doGpioReset();
#endif // CONFIG_WATCHDOG_GPIO_RESET

  private:
  Watchdog watchdog;
  #if CONFIG_WATCHDOG_GPIO_RESET
  uint8_t mGpioTimeoutCnt;
  #endif // CONFIG_WATCHDOG_GPIO_RESET
};

extern tWatchdogProcess WatchdogProcess;

#endif // CONFIG_WATCHDOG
