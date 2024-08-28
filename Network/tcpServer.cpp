#include "../../../global.h"

#if CONFIG_NETWORK

#include "tcpServer.h"

tTcpServer* tTcpServer::pFirst = NULL;

#if CONFIG_TCP_WATCHDOG
tTcpServerProcess TcpServerProcess(CONFIG_TCP_WATCHDOG_TIMEOUT);
#else //CONFIG_TCP_WATCHDOG
tTcpServerProcess TcpServerProcess;
#endif //CONFIG_TCP_WATCHDOG

bool tTcpSession::Process()
{
  if (! mEthernetClient.connected())
  {
    return false;
  }

  // check timeout
  if (0 == mCurrentTimeout--)
  {
     return false;
  }

  return doProcess();
}

void tTcpSession::SendFlashString(const char * str)
{
  SendFlashString(str,strlen_P(str));
}


void tTcpSession::SendFlashString(const char * str, size_t size)
{
  char buffer[BUFFER_SIZE];

  while(size)
  {
    strncpy_P(buffer,str,BUFFER_SIZE);
    str += BUFFER_SIZE;
    if (size >= BUFFER_SIZE)
    {
       mEthernetClient.write(buffer,BUFFER_SIZE);
       size -= BUFFER_SIZE;
    }
    else
    {
       mEthernetClient.write(buffer,size);
       size = 0;
    }
  }
}

void tTcpServerProcess::setup()
{
  tTcpServer* pServer = tTcpServer::GetFirst();
  while (NULL != pServer)
  {
    DEBUG_PRINT_3("Listening on port: ");
    DEBUG_3(println(pServer->GetPort()));
    pServer->setup();
    pServer = pServer->GetNext();
  }

  DEBUG_PRINT_3("server address: ");
  DEBUG_3(println(Ethernet.localIP()));
}

void tTcpServerProcess::service()
{
  tTcpServer* pServer = tTcpServer::GetFirst();
  while (NULL != pServer)
  {
    EthernetClient newClient = pServer->Process();
    if (newClient)
    {
      bool NotFound = true;
      DEBUG_PRINT_3("New connection from ");
      DEBUG_3(print(newClient.remoteIP()));

      // reset watchdog.
      // as Arduino Ethershield likes to hang (HW issue), restart the system if there's no new tcp sessions in last CONFIG_TCP_WATCHDOG_TIMEOUT
      // that requires an external device to make periodic connections
#if CONFIG_TCP_WATCHDOG
      mWatchdog.Reset();
#endif //CONFIG_TCP_WATCHDOG

      for (uint8_t i = 0; i < NUM_OF_CONCURRENT_SESSIONS; i++)
      {
        if (clients[i] == NULL)
        {
            NotFound = false;
            clients[i] = pServer->NewSession(newClient);
            break;
        }
      }

      if (NotFound)
      {
        DEBUG_PRINTLN_3(" rejected - no client slots");
        newClient.stop();
      }
    }
    pServer = pServer->GetNext();
  }

  // proceed all open clients
  for (uint8_t i = 0; i < NUM_OF_CONCURRENT_SESSIONS; i++)
  {
    if (NULL != clients[i])
    {
      bool KeepOpen = clients[i]->Process();
      if (! KeepOpen)
      {
        delete (clients[i]);
        clients[i] = NULL;
      }
    }
  }
}
#endif //CONFIG_NETWORK
