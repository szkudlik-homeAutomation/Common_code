//TODO - split worker to common/sepcific

#include "../../global.h"

#if CONFIG_WORKER_PROCESS

#include "WorkerProcess.h"

WorkerProcess *WorkerProcess::Instance = NULL;

void WorkerProcess::service()
{
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

   uint32_t nextServiceDelay;
   bool continueTask;
   continueTask = pCurrentWorkerTask->Process(&nextServiceDelay);
   if (continueTask)
   {
      // continue after requested time
      setPeriod(nextServiceDelay);
      return;
   }

   // task finished
   delete (pCurrentWorkerTask);
   pCurrentWorkerTask = NULL;
   setPeriod(SERVICE_CONSTANTLY);   // next iteration will go for next queue item or disable the task if the queue is empty
}

void WorkerProcess::Enqueue(WorkerTask *pWorkerTask)
{
   mQueue.enqueue(pWorkerTask);

   if (! isEnabled())
   {
     setPeriod(SERVICE_CONSTANTLY);
     setIterations(RUNTIME_FOREVER);
     enable();
   }
}

#endif //CONFIG_WORKER_PROCESS
