#pragma once

#include "../../../global.h"
#if CONFIG_NETWORK

#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "../WatchdogProcess.h"
#include "../../lib/Ethernet/src/localEthernet.h"

class tTcpSession;
class tTcpServer
{
public:
  static tTcpServer *GetFirst() { return pFirst; }
  tTcpServer *GetNext() { return pNext; }

  void setup() { server.begin(); }
  EthernetClient Process() { return server.accept(); }
  virtual tTcpSession* NewSession(EthernetClient aEthernetClient) = 0;
  uint16_t GetPort() { return server.getPort(); }

protected:
  tTcpServer(uint16_t ServerPort) : server(ServerPort) { pNext = pFirst; pFirst = this; }
  EthernetServer server;

private:
  static tTcpServer* pFirst;
  tTcpServer *pNext;
};


class tTcpSession
{
public:
   // timeout - number of ticks after the session will be closed.
   // unless clearTimeout is called.
  tTcpSession(EthernetClient aEthernetClient, uint16_t timeout)
  {
    mEthernetClient = aEthernetClient;
    mTimeout = timeout;
    clearTimeout();
  }

  virtual ~tTcpSession()
  {
    DEBUG_PRINTLN_3("Session terminated");
    mEthernetClient.stop();
  }

  // if return false - the session should be deleted
  bool Process();

  void SendFlashString(const char * str, size_t size);
  void SendFlashString(const char * str);

  EthernetClient mEthernetClient;

protected:
  // if return false - the session should be deleted
  virtual bool doProcess() = 0;

  void clearTimeout() { mCurrentTimeout = mTimeout; }

private:
   static const uint8_t BUFFER_SIZE = 100; // data portion to be sent in one frame, buffer on stack
   uint16_t mCurrentTimeout;
   uint16_t mTimeout;
};

class tTcpServerProcess : public Process
{
private:

#if CONFIG_TCP_WATCHDOG
	class tWatchdogNetwork : public tWatchdogItem
   {
   public:
      tWatchdogNetwork(uint16_t NumOfSeconds) : tWatchdogItem(NumOfSeconds) {}
      virtual void doRecovery()
      {
         Ethernet.clean();
         DEBUG_PRINTLN_3("=========>!!!!!!! Watchdog for tTcpServerProcess timeout");
      }
   };

public:
  tTcpServerProcess(uint16_t WatchdogTimeout) :
    Process(LOW_PRIORITY,TCP_SERVER_SHEDULER_PERIOD),
    mWatchdog(WatchdogTimeout)
    { }
#else //CONFIG_TCP_WATCHDOG
public:
  tTcpServerProcess() : Process(LOW_PRIORITY,TCP_SERVER_SHEDULER_PERIOD)
    { }
#endif //CONFIG_TCP_WATCHDOG

protected:
  virtual void setup();
  virtual void service();

  static uint8_t const NUM_OF_CONCURRENT_SESSIONS = 4;      // Ethershield can handle 4 session
  static uint8_t const TCP_SERVER_SHEDULER_PERIOD = 10;     //ms
  tTcpSession* clients[NUM_OF_CONCURRENT_SESSIONS];

#if CONFIG_TCP_WATCHDOG
private:
  tWatchdogNetwork mWatchdog;
#endif //CONFIG_TCP_WATCHDOG
};

extern tTcpServerProcess TcpServerProcess;

#endif //CONFIG_NETWORK
