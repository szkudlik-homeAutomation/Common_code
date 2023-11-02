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
};

#endif CONFIG_OUTPUT_CONTROL_SERVLET
