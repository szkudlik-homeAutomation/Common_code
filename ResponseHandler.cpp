#include "../../global.h"
#include "ResponseHandler.h"

ResponseHandler* ResponseHandler::pFirst = NULL;
bool ResponseHandler::mLogsForced = false;

#ifdef DEBUG_SERIAL
ResponseHandlerSerial RespHandler;
#endif

ResponseHandler::~ResponseHandler()
{
  ResponseHandler *i = pFirst;
  ResponseHandler *prev = NULL;
  while(i != NULL)
  {
    if (i == this)
    {
      if (NULL == prev)        // first item?
      {
        pFirst = this->pNext; // next to this becomes first
      }
      else if (NULL == this->pNext) // last item?
      {
        prev->pNext = NULL;   // prev to this becomes last
      }
      else  // middle item
      {
        prev->pNext = this->pNext;
      }
      return; // done
    }
    prev = i;
    i = i->pNext;
  }
}

size_t ResponseHandler::write(uint8_t str)
{
	for (ResponseHandler * i = pFirst; i != NULL ; i = i->pNext)
	{
		if ((i->mLogEnbabled) | mLogsForced) i->vLog(str);
	}
}



void ResponseHandler::VersionResponseHandler(uint8_t SenderID, uint8_t Major, uint8_t Minor, uint8_t Patch)
{
   EnableLogsForce();
   RespHandler.print(F("FW Version for device "));
   RespHandler.print(SenderID,HEX);
   RespHandler.print(F("="));
   RespHandler.print(Major,DEC);
   RespHandler.print(F("."));
   RespHandler.print(Minor,DEC);
   RespHandler.print(F("."));
   RespHandler.print(Patch,DEC);
   RespHandler.println();
   DisableLogsForce();

   for (ResponseHandler * i = pFirst; i != NULL ; i = i->pNext) i->vVersionResponseHandler(SenderID,Major,Minor,Patch);
}


void ResponseHandler::NodeScanResponse(uint32_t ActiveNodesMap)
{
	EnableLogsForce();
	RespHandler.print(F("Active node map: "));
	RespHandler.println(ActiveNodesMap,BIN);
	DisableLogsForce();

	for (ResponseHandler * i = pFirst; i != NULL ; i = i->pNext) i->vNodeScanResponse(ActiveNodesMap);
}
