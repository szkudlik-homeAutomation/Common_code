#include "WatchdogProcess.h"
#include <Watchdog.h>
#include "../lib/external/Ethernet/src/localEthernet.h"

tWatchdogItem *tWatchdogItem::pFirst = NULL;

bool tWatchdogItem::Tick() {
   if (0 == mNumOfSeconds) return true;   // watchdog item disabled
   if (mCurrentTimeoutCnt > 0)
   {
      mCurrentTimeoutCnt--;
   }
   else
   {
      Ethernet.clean();
      Reset();
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
      doResetWatchdog &= pItem->Tick();
      pItem = pItem->pNext;
   }

   watchdog.reset();

   if (doResetWatchdog)
   {
//      watchdog.reset();
   }
   else
   {
      DEBUG_PRINTLN_3("Watchdog goes off!");
   }
}
