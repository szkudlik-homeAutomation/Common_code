#include "../../global.h"

#if CONFIG_WATCHDOG

#include "WatchdogProcess.h"
#include <Watchdog.h>
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
   }
}
#endif //CONFIG_WATCHDOG
