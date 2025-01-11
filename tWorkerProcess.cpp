//TODO - split worker to common/sepcific

#include "../../global.h"

#if CONFIG_WORKER_PROCESS

#include "tWorkerProcess.h"
#include "tMessageReciever.h"

tWorkerProcess *tWorkerProcess::Instance = NULL;
void tWorkerProcess::service()
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
   if (pCurrentWorkerTask->cookie != 0)
   {
	   tWorkerTaskFinishedEvent event;
       event.pTask = pCurrentWorkerTask;
       tMessageReciever::Dispatch(MessageType_WorkerTaskFinishedEvent, pCurrentWorkerTask->cookie, &event);
   }

   delete (pCurrentWorkerTask);
   pCurrentWorkerTask = NULL;
   setPeriod(SERVICE_CONSTANTLY);   // next iteration will go for next queue item or disable the task if the queue is empty
}

void tWorkerProcess::Enqueue(WorkerTask *pWorkerTask, uint16_t cookie)
{
	pWorkerTask->cookie = cookie;
   mQueue.enqueue(pWorkerTask);

   if (! isEnabled())
   {
     setPeriod(SERVICE_CONSTANTLY);
     setIterations(RUNTIME_FOREVER);
     enable();
   }
}

#endif //CONFIG_WORKER_PROCESS
