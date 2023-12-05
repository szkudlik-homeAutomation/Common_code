#include "../../../global.h"

#if CONFIG_TLE8457_COMM_LIB
#if CONFIG_NODE_SCAN_TASK

#include "tOutgoingFrames.h"
#include "TLE8457_serial_lib.h"
#include "NodeScanTask.h"


bool NodeScanTask::Process(uint32_t *pNextServiceDelay)
{
   *pNextServiceDelay = REQUEST_SENDING_PERIOD;

   if (mCurrentNodeID < MAX_NUM_OF_NODES)
   {
       // send a frame
       mCurrentNodeID++;
       tOutgoingFrames::SendMsgVersionRequest(mCurrentNodeID);  // staring from 1
   }
   else if (mCurrentNodeID == MAX_NUM_OF_NODES)
   {
      *pNextServiceDelay = RESPONSE_WAIT_PERIOD;
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
	if (type != MessageType_SerialFrameRecieved || data != MESSAGE_TYPE_FW_VERSION_RESPONSE)
		return;

    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
	mActiveNodesMap |= 1 << (pFrame->SenderDevId - 1);
}

#endif //CONFIG_NODE_SCAN_TASK
#endif //CONFIG_TLE8457_COMM_LIB
