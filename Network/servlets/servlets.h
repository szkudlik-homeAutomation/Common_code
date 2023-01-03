#pragma once

#include "../../../../global.h"
#if CONFIG_HTTP_SERVLET_OUTCONTROL_JS

#include "../../../http_binaries.h"

#include "../httpServer.h"


#ifdef OutputControl_js_raw_len

class tOutputControlJavaScript : public tHttpServlet
{
public:
  tOutputControlJavaScript() : tHttpServlet() {}
  virtual ~tOutputControlJavaScript() {}

  virtual bool ProcessAndResponse();
};

#endif

#endif //CONFIG_HTTP_SERVLET_OUTCONTROL_JS
