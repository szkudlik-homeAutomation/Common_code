#pragma once

#include "../../global.h"

#if CONFIG_WORKER_PROCESS


#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../lib/ArduinoQueue/ArduinoQueue.h"

class WorkerTask
{
public:
   WorkerTask() {}
   virtual ~WorkerTask() {}

   virtual bool Process(uint32_t * pPeriod) = 0;
};


class WorkerProcess: public Process, public ResponseHandler
{
  public:
  WorkerProcess(Scheduler &manager) : Process(manager,LOW_PRIORITY,SERVICE_SECONDLY,RUNTIME_FOREVER), pCurrentWorkerTask(NULL) {}

  void Enqueue(WorkerTask *pWorkerTask);

protected:
  virtual void service();
private:

  ArduinoQueue<WorkerTask *> mQueue;
  WorkerTask *pCurrentWorkerTask;

};

extern WorkerProcess Worker;

#endif CONFIG_WORKER_PROCESS
