#pragma once

#include "../../global.h"
#include "../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include <ArduinoQueue.h>

class WorkerTask
{
public:
   WorkerTask() {}
   virtual ~WorkerTask() {}

   virtual bool Process(uint32_t * pPeriod) = 0;
};


class WorkerProcessBase: public Process
{
public:
   WorkerProcessBase(Scheduler &manager) : Process(manager,LOW_PRIORITY,SERVICE_SECONDLY,RUNTIME_FOREVER), pCurrentWorkerTask(NULL) {}
protected:
  virtual void service();

  void Enqueue(WorkerTask *pWorkerTask);
private:

  ArduinoQueue<WorkerTask *> mQueue;
  WorkerTask *pCurrentWorkerTask;

};

