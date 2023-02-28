/*
 * tMessage.h
 *
 *  Created on: Feb 28, 2023
 *      Author: mszkudli
 */

#pragma once

#include "../../global.h"

class tMessageReciever {
public:
	tMessageReciever()  { pNext = pFirst ; pFirst = this; }
	~tMessageReciever();

	void RegisterMessageType(uint8_t type)   { mMessageMask |= 1 << (type - 1); }
	void UnRegisterMessageType(uint8_t type) { mMessageMask &=  ~(uint32_t)(1 << (type - 1)); }

	static void Dispatch(uint8_t type, void *data);
protected:
	virtual void onMessage(uint8_t type, void *data) = 0;
private:
  static tMessageReciever* pFirst;
  tMessageReciever* pNext;

  uint32_t mMessageMask;
};

