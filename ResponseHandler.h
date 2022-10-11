#ifndef RESPONSE_HANDLER
#define RESPONSE_HANDLER
#include "../../global.h"
#include "Print.h"

class ResponseHandler : public Print
{
public:
  ResponseHandler() : mLogEnbabled(true) { pNext = pFirst ; pFirst = this; }

  virtual ~ResponseHandler();

  void EnableLogs() { mLogEnbabled = true; }
  void DisableLogs() { mLogEnbabled = false; }
  static void EnableLogsForce() { mLogsForced = true; }
  static void DisableLogsForce() { mLogsForced = false; }

  virtual size_t write(uint8_t str);

  static void VersionResponseHandler(uint8_t SenderID, uint8_t Major, uint8_t Minor, uint8_t Patch);
  static void NodeScanResponse(uint32_t ActiveNodesMap);

protected:

  virtual void vLog(uint8_t str){}

  virtual void vVersionResponseHandler(uint8_t DevID, uint8_t Major, uint8_t Minor, uint8_t Patch) {}

  virtual void vNodeScanResponse(uint32_t ActiveNodesMap) {}

private:
  bool mLogEnbabled;
  static bool mLogsForced;
  static ResponseHandler* pFirst;
  ResponseHandler* pNext;
};

class ResponseHandlerSerial : public ResponseHandler
{
public:
  ResponseHandlerSerial() : ResponseHandler() {}
  virtual ~ResponseHandlerSerial() {};

protected:
#ifdef DEBUG_SERIAL
  virtual void vLog(uint8_t str) { DEBUG_SERIAL.write(str); }
#else
  virtual void vLog(uint8_t str) { }
#endif

};

extern ResponseHandlerSerial RespHandler;
#endif // RESPONSE_HANDLER
