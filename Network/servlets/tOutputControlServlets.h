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
#include "../../OutputProcess.h"

/**
 * output servlet - controlling or checking state of an output
 */
class tOutputSetServlet :  public tHttpServlet
{
public:
  tOutputSetServlet(tOutputProcess *aOutputProcess) :
	  pOutputProcess(aOutputProcess),
	  tHttpServlet() {}
  virtual ~tOutputSetServlet() {}

  virtual bool ProcessAndResponse();
private:
  tOutputProcess *pOutputProcess;
};

class tOutputStateServlet : public tHttpServlet
{
  public:
   tOutputStateServlet(tOutputProcess *aOutputProcess)  :
  	  pOutputProcess(aOutputProcess),
  	  tHttpServlet() {}

   virtual ~tOutputStateServlet() {}

   virtual bool ProcessAndResponse();
  private:
    tOutputProcess *pOutputProcess;
};

#endif CONFIG_OUTPUT_CONTROL_SERVLET
