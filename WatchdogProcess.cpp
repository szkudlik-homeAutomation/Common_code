#include "../../global.h"

#if CONFIG_WATCHDOG

#include "WatchdogProcess.h"
#include "../lib/Ethernet/src/localEthernet.h"

tWatchdogItem *tWatchdogItem::pFirst = NULL;

bool tWatchdogItem::Tick() {
   if (0 == mNumOfSeconds) return true;   // watchdog item disabled
   if (mCurrentTimeoutCnt > 0)
   {
      mCurrentTimeoutCnt--;
   }
   else
   {
      return false;  // timeout
   }
   return true;
}


void tWatchdogProcess::service()
{
   tWatchdogItem *pItem = tWatchdogItem::pFirst;
   bool doResetWatchdog = true;

   while (NULL != pItem)
   {
      bool isOK = pItem->Tick();
      doResetWatchdog &= isOK;
      if (! isOK)
      {
         // an item has not been reset in required time
         // try reset...
         pItem->doRecovery();
      }
      pItem = pItem->pNext;
   }

   if (doResetWatchdog)
   {
      // set in HW that everything is fine
      watchdog.reset();
      #if CONFIG_WATCHDOG_GPIO_RESET
      mGpioTimeoutCnt = 0;
      #endif // CONFIG_WATCHDOG_GPIO_RESET
   }
   #if CONFIG_WATCHDOG_GPIO_RESET
   else
   {
      mGpioTimeoutCnt++;
      if (mGpioTimeoutCnt >= CONFIG_WATCHDOG_GPIO_RESET_TIMEOUT)
      {
         doGpioReset();
      }
   }
   #endif // CONFIG_WATCHDOG_GPIO_RESET
   
}

#if CONFIG_WATCHDOG_GPIO_RESET
void tWatchdogProcess::doGpioReset()
{

   digitalWrite(CONFIG_WATCHDOG_GPIO_RESET_PIN, LOW);
   pinMode(CONFIG_WATCHDOG_GPIO_RESET_PIN, OUTPUT);   // set the GPIO pin to output mode for triggering the reset pulse (open-drain, strong pull down)

   // wait for the reset pulse to be recognized by the hardware
   // note - if the gpio reset pin reset CPU this delay may not be reached
   delay(CONFIG_WATCHDOG_GPIO_RESET_PULSE_TIME_MS);

   pinMode(CONFIG_WATCHDOG_GPIO_RESET_PIN, INPUT); // set back to input mode after the reset pulse (weak pull-up)
}
#endif // CONFIG_WATCHDOG_GPIO_RESET

void tWatchdogProcess::resetSystem()
{
   #if CONFIG_WATCHDOG_GPIO_RESET
   doGpioReset();
   #endif // CONFIG_WATCHDOG_GPIO_RESET

   // stop interrupts and wait for HW watchdog to reset the system
   noInterrupts();
   while (true) {}
}

#endif //CONFIG_WATCHDOG
