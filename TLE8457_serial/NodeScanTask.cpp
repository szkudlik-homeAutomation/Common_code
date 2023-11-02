#include "../../../global.h"

#if CONFIG_TLE8457_COMM_LIB
#if CONFIG_NODE_SCAN_TASK

#include "tOutgoingFrames.h"
#include "TLE8457_serial_lib.h"
#include "NodeScanTask.h"


bool NodeScanTask::Process(uint32_t * pPeriod)
{
   *pPeriod = REQUEST_SENDING_PERIOD;

   if (mCurrentNodeID < MAX_NUM_OF_NODES)
   {
       // send a frame
       mCurrentNodeID++;
       tOutgoingFrames::SendMsgVersionRequest(mCurrentNodeID);  // staring from 1
   }
   else if (mCurrentNodeID == MAX_NUM_OF_NODES)
   {
      *pPeriod = RESPONSE_WAIT_PERIOD;
       mCurrentNodeID++;
   }
   else
   {
       // send result
	   LOG_PRINT("Active node map: ");
	   LOG(println(mActiveNodesMap,BIN));

	   //TODO: send a message
       return false;
   }

   return true;
}

void NodeScanTask::onMessage(uint8_t type, uint16_t data, void *pData)
{
	if (data != tMessages::ExternalEvent_VersionResponse)
		return;

	struct tMessages::tVersionResponse *pVersionResponse = (struct tMessages::tVersionResponse *)pData;
	mActiveNodesMap |= 1 << (pVersionResponse->SenderID - 1);
}

#endif //CONFIG_NODE_SCAN_TASK
#endif //CONFIG_TLE8457_COMM_LIB
