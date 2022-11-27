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
      doResetWatchdog &= pItem->Tick();
      pItem = pItem->pNext;
   }

   if (doResetWatchdog)
   {
      watchdog.reset();
   }
   else
   {
      Ethernet.clean();
      DEBUG_PRINTLN_3("=========>!!!!!!! Watchdog timeout");
   }
}
