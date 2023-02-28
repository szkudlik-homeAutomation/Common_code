/*
 * tMessage.cpp
 *
 *  Created on: Feb 28, 2023
 *      Author: mszkudli
 */

#include "../../global.h"

#include "tMessageReciever.h"

tMessageReciever * tMessageReciever::pFirst = NULL;

tMessageReciever::~tMessageReciever()
{
  tMessageReciever *i = pFirst;
  tMessageReciever *prev = NULL;
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

void tMessageReciever::Dispatch(uint8_t type, void *data)
{
	tMessageReciever *pCurrent = pFirst;
	while (pCurrent != NULL)
	{
		if (pCurrent->mMessageMask & type)
		{
			pCurrent->onMessage(type,data);
		}
	}
}
