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
  tWatchdogProcess() : Process(WATCHDOG_PRIORITY, SERVICE_SECONDLY, RUNTIME_FOREVER) {}
  virtual void setup()
  {
    watchdog.enable(Watchdog::TIMEOUT_8S);
  }
  virtual void service();

  private:
  Watchdog watchdog;
};

extern tWatchdogProcess WatchdogProcess;

#endif // CONFIG_WATCHDOG
