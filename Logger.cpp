/*
 * Logger.cpp
 *
 *  Created on: Jan 12, 2023
 *      Author: mszkudli
 */


#include "../../global.h"
#if CONFIG_LOGGER

#include "Logger.h"

tLogTransport* tLogTransport::pFirst = NULL;
tLogger *tLogger::Instance = NULL;
tSerialLogTransport *tSerialLogTransport::Instance = NULL;

tLogTransport::~tLogTransport()
{
  tLogTransport *i = pFirst;
  tLogTransport *prev = NULL;
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

size_t tLogger::write(uint8_t str)
{
	for (tLogTransport * i = (tLogTransport*)tLogTransport::pFirst; i != NULL ; i = (tLogTransport*)i->pNext)
	{
		if ((i->mLogEnbabled) | mLogsForced) i->Log(str);
	}
}


tLogger Logger;

#ifdef CONFIG_LOGGER_SERIAL
tSerialLogTransport SerialLogTransport;
#endif


#endif // CONFIG_LOGGER
