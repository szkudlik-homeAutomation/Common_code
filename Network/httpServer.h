#pragma once

#include "../../../global.h"
#if CONFIG_HTTP_SERVER

#include "tcpServer.h"

class tHttpSession;

class tHttpServlet
{
  public:
   tHttpServlet() : pOwner(NULL) {}
   virtual ~tHttpServlet() {}

   virtual bool ProcessAndResponse() = 0;

  protected:
    tHttpSession *pOwner;

    void SendResponse400();
    void SendResponse424();
    void SendResponse200();
    void SendResponse501();
    void SendResponse503();
    void SendVersionAndPageClose();

    bool GetParameter(const char * Param) { return (NULL != FindParameter(Param,NULL,NULL)); }
    bool GetParameter(const char * Param, unsigned *pValue);


  private:
   char * tHttpServlet::FindParameter(const char * Param,  char **ppValue, uint8_t *pValueLen);
   friend class tHttpSession; void SetOwner(tHttpSession *owner) { pOwner = owner; }
};


class tHttpSession : public tTcpSession
{
public:
  static uint16_t const HTTP_SESSION_TIMEOUT = 1000;
  tHttpSession(EthernetClient aEthernetClient);
  virtual ~tHttpSession() { if (pServlet) delete pServlet; }

  String RequestBuffer;

protected:
   virtual bool doProcess();

private:
   /**
    * Pointer to servlet providing data
    */
   tHttpServlet *pServlet;
   static const uint8_t HTTP_MAX_REQUEST_BUFFER_SIZE = 200;
};



class tHttpServer : public tTcpServer
{
public:
  static tHttpServer *Instance;
  tHttpServer() : tTcpServer(80) { Instance = this; }

  tHttpServlet *newServlet(String *pRequestBuffer)
  {
	  tHttpServlet *pServlet = ServletFactory(pRequestBuffer);
	  if (NULL == pServlet)
		  pServlet = DefaultServletFactory(pRequestBuffer);
	  if (NULL == pServlet)
		  pServlet = getDefaultServlet(pRequestBuffer);
	  return pServlet;
  }

protected:
  bool isServlet(const String &servlet) const;
  virtual tTcpSession *NewSession(EthernetClient aEthernetClient) { return new tHttpSession(aEthernetClient); }

  /* user - specific servlet factory */
  virtual tHttpServlet *ServletFactory(String *pRequestBuffer) { return NULL; }

  /* default servlet - in case of a page not found */
  virtual tHttpServlet *getDefaultServlet(String *pRequestBuffer) { return NULL; }

private:
  tHttpServlet *DefaultServletFactory(String *pRequestBuffer);
};

#endif //CONFIG_HTTP_SERVER
