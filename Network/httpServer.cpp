#include "../../../global.h"
#if CONFIG_HTTP_SERVER
#include "httpServer.h"
#include "servlets/tSensorStateServlet.h"
#include "servlets/tOutputControlServlets.h"

tHttpServer *tHttpServer::Instance = NULL;


tHttpServlet *tHttpServer::DefaultServletFactory(String *pRequestBuffer)
{
#if CONFIG_SENSOR_STATE_SERVLET
   if (pRequestBuffer->startsWith("/sensorState")) return new tSensorStateServlet();
#endif // CONFIG_SENSOR_STATE_SERVLET

#if CONFIG_OUTPUT_CONTROL_SERVLET
#if CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL
   if (pRequestBuffer->startsWith("/outputState")) return new tOutputStateTLE8457Servlet();
#else // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL
   if (pRequestBuffer->startsWith("/outputState")) return new tOutputStateServlet();
#endif // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL

   if (pRequestBuffer->startsWith("/outputSet")) return new tOutputSetServlet();
#endif
   return NULL;
}

void tHttpServlet::SendResponse400()
{
   pOwner->SendFlashString(PSTR("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\n400 Bad request\r\nVersion: "));
   pOwner->SendFlashString(PSTR(FW_VERSION));
   pOwner->SendFlashString(PSTR("\r\n"));
}

void tHttpServlet::SendResponse200()
{
  pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n200 OK\r\nVersion: "));
  pOwner->SendFlashString(PSTR(FW_VERSION));
  pOwner->SendFlashString(PSTR("\r\n"));
}

void tHttpServlet::SendResponse501()
{
	  pOwner->SendFlashString(PSTR("HTTP/1.1 501 Not Implemented\r\nContent-Type: text/plain\r\n\r\n501 Not implemented\r\nVersion: "));
	  pOwner->SendFlashString(PSTR(FW_VERSION));
	  pOwner->SendFlashString(PSTR("\r\n"));
}

void tHttpServlet::SendResponse503()
{
	  pOwner->SendFlashString(PSTR("HTTP/1.1 503 Server busy\r\nContent-Type: text/plain\r\n\r\n503 BUSY\r\nVersion: "));
	  pOwner->SendFlashString(PSTR(FW_VERSION));
	  pOwner->SendFlashString(PSTR("\r\n"));
}

void tHttpServlet::SendResponse424()
{
	pOwner->SendFlashString(PSTR("HTTP/1.1 424 OK\r\nContent-Type: text/plain\r\n\r\n424 No data from remote device\r\nVersion: "));
	pOwner->SendFlashString(PSTR(FW_VERSION));
	pOwner->SendFlashString(PSTR("\r\n"));
}

bool tHttpServlet::GetParameter(const char * Param, uint16_t *pValue)
{
  char *pParamString = NULL;
  uint8_t ParamValueLen = 0;
  if (NULL == FindParameter(Param,&pParamString,&ParamValueLen))
  {
    return false;
  }

  // get int value of a param
  uint16_t ParamValue = 0;
  for (uint8_t i = 0; i < ParamValueLen; i++)
  {
    char digit = *(pParamString+i);
    if ((digit < '0') || (digit > '9')) return false;
    ParamValue *= 10;
    ParamValue += digit - '0';
  }
  *pValue = ParamValue;
  return true;
}

char * tHttpServlet::FindParameter(const char * Param,  char **ppValue, uint8_t *pValueLen)
{
  // request buffer  points to 1st parameter
  int Pos = 0;

  // find params
  while (Pos < pOwner->RequestBuffer.length())
  {
    char *Buf = pOwner->RequestBuffer.c_str()+Pos;

    // find a separator
    char * pAmpSeparator = strchr(Buf,'&');
    uint8_t ParamLen = strlen(Buf);
    if (NULL != pAmpSeparator)  // if not last param
    {
      ParamLen = pAmpSeparator - Buf;
    }
    if (ParamLen == 0) return NULL;

    uint8_t ParamNameLen = ParamLen;
    char * pEqSeparator = strchr(Buf,'=');
    if (NULL != pEqSeparator)  // if param has a value
    {
      ParamNameLen = pEqSeparator - Buf;
      if (ParamNameLen > ParamLen) ParamNameLen = ParamLen;
    }

    // check param name
    if ((strlen(Param) == ParamNameLen) &&
        (0 == strncmp(Buf,Param,ParamNameLen)) )
    {
      // found
      if (ParamNameLen)
      {
        if (NULL != ppValue) *ppValue = Buf+ParamNameLen+1;
        if (NULL != pValueLen) *pValueLen = ParamLen - ParamNameLen - 1 ;
      }

      return Buf;
    }

    // next param
    Pos += ParamLen+1;
  }

  return NULL;
}

tHttpSession::tHttpSession(EthernetClient aEthernetClient) : tTcpSession(aEthernetClient, HTTP_SESSION_TIMEOUT), pServlet(NULL)
{
  DEBUG_PRINTLN_3("HTTP Session started");
}

bool tHttpSession::doProcess()
{
  // is the servlet already found?
  if (NULL != pServlet) return pServlet->ProcessAndResponse();

  // no servlet yet - parse the request

  // read the part of request - till the buffer size or end of request URL
  bool RequestCompleted = false;
  while ( mEthernetClient.available() && (RequestBuffer.length() <= HTTP_MAX_REQUEST_BUFFER_SIZE) )
  {
      char c = mEthernetClient.read();
      if (c == '\r')
      {
        RequestCompleted = true;
        break;  // request (1st line) completed
      }
      RequestBuffer += c;
  }

  if ((false == RequestCompleted) && (RequestBuffer.length() <= HTTP_MAX_REQUEST_BUFFER_SIZE))
  {
    // wait for more data
    return true;
  }

  // find if the request starts with GET
  if (! RequestBuffer.startsWith("GET "))
  {
    return false; // drop the session
  }
  RequestBuffer.remove(0,4);  // 4 = size of "get "
  // find the servlet
  pServlet = tHttpServer::Instance->newServlet(&RequestBuffer);

  if (NULL == pServlet)
  {
    // if not found - return 404

    SendFlashString(PSTR("HTTP/1.1 404 Not found\r\nContent-Type: text/plain\r\n\r\n404 Not found\r\n"));
    mEthernetClient.println(RequestBuffer);
    SendFlashString(PSTR("\r\n\r\nVersion: "));
    SendFlashString(PSTR(FW_VERSION));
    SendFlashString(PSTR("\r\n"));
    return false; // close the session
  }

  pServlet->SetOwner(this);
  // remove everything before ? from request buffer (leave parameters only)
  int ParamsPos = RequestBuffer.indexOf("?");
  if (ParamsPos>0)
    RequestBuffer.remove(0,ParamsPos+1);
  // leave parameters only - find space as a separator
  ParamsPos = RequestBuffer.indexOf(" ");
  if (ParamsPos>0)
    RequestBuffer.remove(ParamsPos,10000);

  return true;  // go for next iteration
}

void tHttpServlet::SendVersionAndPageClose()
{
   pOwner->SendFlashString(PSTR("<br><small><b><i>Version: "));
   pOwner->SendFlashString(PSTR(FW_VERSION));
   pOwner->SendFlashString(PSTR("</body></html>"));
}

#endif //CONFIG_HTTP_SERVER
