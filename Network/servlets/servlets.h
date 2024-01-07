#pragma once

#include "../../../../global.h"
#include "../httpServer.h"

#if CONFIG_HTTP_SERVER

#if CONFIG_HTTP_SERVLET_OUTCONTROL_JS
#include "../../../http_binaries.h"

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

#if CONFIG_TIMESTAMP_SERVLET
class tTimestampServlet : public tHttpServlet
{
public:
    tTimestampServlet() : tHttpServlet() {}
  virtual ~tTimestampServlet() {}

  virtual bool ProcessAndResponse();
};
#endif //CONFIG_TIMESTAMP_SERVLET

#endif // CONFIG_HTTP_SERVER
