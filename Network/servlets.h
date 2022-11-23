#pragma once

#include "../../../global.h"
#include "../../http_binaries.h"

#include "httpServer.h"


#ifdef OutputControl_js_raw_len

class tOutputControlJavaScript : public tHttpServlet
{
public:
  tOutputControlJavaScript() : tHttpServlet() {}
  virtual ~tOutputControlJavaScript() {}

  virtual bool ProcessAndResponse();
};

#endif

