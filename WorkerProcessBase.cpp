//TODO - split worker to common/sepcific

#include "../global.h"
#include "WorkerProcessBase.h"
#include "ResponseHandler.h"

void WorkerProcessBase::service()
{
  #ifdef CONTROLLER
   if (NULL == pCurrentWorkerTask)
   {
      if (mQueue.isEmpty())
      {
         // go to sleep
         disable();
         return;
      }

      pCurrentWorkerTask = mQueue.dequeue();
   }

   uint32_t nextPeriod;
   bool continueTask;
   continueTask = pCurrentWorkerTask->Process(&nextPeriod);
   if (continueTask)
   {
      // continue after requested time
      setPeriod(nextPeriod);
      return;
   }

   // task finished
   delete (pCurrentWorkerTask);
   pCurrentWorkerTask = NULL;
   setPeriod(SERVICE_CONSTANTLY);   // next iteration will go for next queue item or disable the task if the queue is empty
#else
   disable();
#endif
}

void WorkerProcessBase::Enqueue(WorkerTask *pWorkerTask)
{
   mQueue.enqueue(pWorkerTask);

   if (! isEnabled())
   {
     setPeriod(SERVICE_CONSTANTLY);
     setIterations(RUNTIME_FOREVER);
     enable();
   }
}
