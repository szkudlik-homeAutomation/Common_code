#ifndef TELNET_SERVER
#define TELNET_SERVER
#include "../global.h"
#include "lib/TCP_Communication_lib/tcpServer.h"
#include "ResponseHandler.h"

class commandList_t;

class tTelnetSession : public tTcpSession, public ResponseHandler
{
public:
  tTelnetSession(EthernetClient aEthernetClient,commandList_t *pTelnetCommands, uint8_t NumOfTelnetCommands);
  virtual ~tTelnetSession();

protected:
  virtual bool doProcess();

  virtual void vLog(uint8_t str);
private:
  static const uint16_t TELNET_SESSION_TIMEOUT = 65535; // 10 minutes
};


class tTelnetServer : public tTcpServer
{
public:
  tTelnetServer(commandList_t *pTelnetCommands, uint8_t NumOfTelnetCommands) :
        tTcpServer(23),
        mpTelnetCommands(pTelnetCommands),
        mNumOfTelnetCommands(NumOfTelnetCommands)
        {}

  void EnableLogs();
  void DisableLogs();

protected:
  virtual tTcpSession* NewSession(EthernetClient aEthernetClient) { return new tTelnetSession(aEthernetClient,mpTelnetCommands,mNumOfTelnetCommands); }
private:
  commandList_t *mpTelnetCommands;
  uint8_t mNumOfTelnetCommands;
};



#endif  // WORKER_PROCESS
