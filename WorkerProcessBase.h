#ifndef WORKER_PROCESS_BASE
#define WORKER_PROCESS_BASE
#include "../global.h"
#include <ProcessScheduler.h>
#include "ResponseHandler.h"
#include <ArduinoQueue.h>

class WorkerTask
{
public:
   WorkerTask() {}
   virtual ~WorkerTask() {}

   virtual bool Process(uint32_t * pPeriod) = 0;
};


class WorkerProcessBase: public Process, public ResponseHandler
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


#endif  // WORKER_PROCESS_BASE
