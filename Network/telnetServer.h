#pragma once

#include "../../../global.h"
#if CONFIG_TELNET_SERVER

#include "tcpServer.h"
#include "../Logger.h"
#include "../../lib/Commander/src/Commander.h"
#include "../tMessageReciever.h"
#include "../tMessages.h"

class commandList_t;

class tTelnetSession : public tTcpSession, public tLogTransport, public tMessageReciever
{
public:
  tTelnetSession(EthernetClient aEthernetClient,commandList_t *pTelnetCommands, uint8_t NumOfTelnetCommands);
  virtual ~tTelnetSession();

protected:
  virtual bool doProcess();
  virtual void onMessage(uint8_t type, uint16_t data, void *pData);

  virtual void Log(uint8_t str);
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

bool TelnetEnableLogs(Commander &Cmdr);
bool TelnetDisableLogs(Commander &Cmdr);
#if CONFIG_TLE8457_COMM_LIB
bool send_GetVersion(Commander &Cmdr);
bool send_Reset(Commander &Cmdr);
#if CONFIG_OUTPUT_PROCESS
bool send_stateOverviewHandler(Commander &Cmdr);
bool send_OutputStateHandler(Commander &Cmdr);
bool send_SetOutputHandler(Commander &Cmdr);
#endif CONFIG_OUTPUT_PROCESS

#endif //CONFIG_TLE8457_COMM_LIB

#endif //CONFIG_TELNET_SERVER
