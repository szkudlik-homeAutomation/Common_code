/*
 * tOutputSetServlet.h
 *
 *  Created on: Dec 9, 2022
 *      Author: mszkudli
 */

#pragma once

#include "../../../../global.h"

#if CONFIG_OUTPUT_CONTROL_SERVLET

#include "servlets.h"
#include "../../tOutputProcess.h"
#include "../HttpServer.h"

/**
 * output servlet - controlling or checking state of an output
 */
class tOutputSetServlet :  public tHttpServlet
{
public:
  tOutputSetServlet() :
	  tHttpServlet() {}
  virtual ~tOutputSetServlet() {}

  virtual bool ProcessAndResponse();
};

class tOutputStateServlet : public tHttpServlet
{
  public:
   tOutputStateServlet(): tHttpServlet() {}

   virtual ~tOutputStateServlet() {}

   virtual bool ProcessAndResponse();
 protected:
   void sendJsonResponse(uint16_t devID, uint16_t Output, uint16_t State, uint16_t TimerValue, uint16_t DefaultTimer);
};


#if CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL

class tOutputStateTLE8457Servlet : public tOutputStateServlet, public tMessageReciever
{
  public:
	tOutputStateTLE8457Servlet() :  tOutputStateServlet(), mRequestSent(false)
  {
	  RegisterMessageType(MessageType_SerialFrameRecieved);
  }

  virtual ~tOutputStateTLE8457Servlet() {}
  virtual void onMessage(uint8_t type, uint16_t data, void *pData);
  virtual bool ProcessAndResponse();

protected:
  uint8_t mExpectedDevID;
  uint8_t mExpectedOutputID;
  unsigned long mStartTimestamp;
  uint8_t   mPowerState;
  uint16_t  mTimerValue;
  uint16_t  mDefaultTimer;

  static uint8_t const STATUS_WAIT = 0;
  static uint8_t const STATUS_TIMEOUT = 1;
  static uint8_t const STATUS_COMPLETE = 2;

  void SendOutputStateRequest(uint8_t DevID, uint8_t OutputID);
  uint8_t CheckStateRequest();

  bool mRequestSent;
};

#endif // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL

#endif CONFIG_OUTPUT_CONTROL_SERVLET
