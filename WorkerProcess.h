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

   virtual bool Process(uint32_t * pNextServiceDelay) = 0;
};


class WorkerProcess: public Process
{
  public:
	static WorkerProcess *Instance;
    WorkerProcess() : Process(LOW_PRIORITY,SERVICE_SECONDLY,RUNTIME_FOREVER), pCurrentWorkerTask(NULL) { Instance = this; }

    void Enqueue(WorkerTask *pWorkerTask);

protected:
  virtual void service();
private:

  ArduinoQueue<WorkerTask *> mQueue;
  WorkerTask *pCurrentWorkerTask;

};

#endif CONFIG_WORKER_PROCESS
