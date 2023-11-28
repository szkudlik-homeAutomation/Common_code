#pragma once

#include "../../../global.h"
#if CONFIG_TLE8457_COMM_LIB
#if CONFIG_NODE_SCAN_TASK

#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../../lib/ArduinoQueue/ArduinoQueue.h"
#include "../WorkerProcess.h"
#include "../tMessageReciever.h"

class NodeScanTask : public WorkerTask, public tMessageReciever
{
public:
   NodeScanTask() : mCurrentNodeID(0), mActiveNodesMap(0)
   {
	   RegisterMessageType(MessageType_SerialFrameRecieved);
   }
   virtual ~NodeScanTask() {}

   virtual bool Process(uint32_t * pNextServiceDelay);
   static void trigger() { Worker.Enqueue(new NodeScanTask()); }

protected:
   virtual void onMessage(uint8_t type, uint16_t data, void *pData);

private:
   static const uint16_t REQUEST_SENDING_PERIOD = 600;  // 600ms
   static const uint16_t RESPONSE_WAIT_PERIOD   = 2000;  // 2s

   uint8_t mCurrentNodeID;  // if == MAX_NUM_OF_NODES - waiting for responses
   uint32_t mActiveNodesMap;
};
#endif // CONFIG_NODE_SCAN_TASK
#endif //CONFIG_TLE8457_COMM_LIB
