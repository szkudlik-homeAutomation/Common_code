#include "telnetServer.h"

#include "../../../global.h"
#if CONFIG_TELNET_SERVER
#include "../TLE8457_serial/tOutgoingFrames.h"
#include "../TLE8457_serial/CommonFramesDefs.h"

#if CONFIG_OUTPUT_PROCESS
#include "../OutputProcess.h"
#endif //CONFIG_OUTPUT_PROCESS

// must be static-global (why? - only 1 telnet session may be active)
Commander cmd;

tTelnetSession *pTelnetSession = NULL;

tTelnetSession::tTelnetSession(EthernetClient aEthernetClient,commandList_t *pTelnetCommands, uint8_t NumOfTelnetCommands)
   : tTcpSession(aEthernetClient, TELNET_SESSION_TIMEOUT),
	 tLogTransport(),
	 tMessageReciever()
{
  DEBUG_PRINTLN_3("TELNET Session started");
  RegisterMessageType(tMessages::MessageType_ExternalEvent);
  cmd.begin(&mEthernetClient, pTelnetCommands, NumOfTelnetCommands);
  cmd.commandPrompt(ON); //enable the command prompt
  cmd.echo(false);     //Echo incoming characters to theoutput port
  cmd.errorMessages(ON); //error messages are enabled - it will tell us if we issue any unrecognised commands
  cmd.autoChain(ON);
  cmd.printCommandList();
  uint16_t FreeRam = getFreeRam();
  cmd.print(F("Version: "));
  cmd.print(F(FW_VERSION));
  cmd.print(F(" FreeRam: "));
  cmd.println(FreeRam);
  pTelnetSession = this;
  DisableLogs();
}

bool tTelnetSession::doProcess()
{
  cmd.update();
  return true;
}

tTelnetSession::~tTelnetSession() { pTelnetSession = NULL; }

void tTelnetSession::Log(uint8_t str)
{
    if (NULL != cmd.getOutputPort())
      cmd.getOutputPort()->write(str);
}

void tTelnetSession::onMessage(uint8_t type, uint16_t data, void *pData)
{
	struct tMessages::tVersionResponse *pVersionResponse = (struct tMessages::tVersionResponse *)pData;
	struct tMessages::tOutputStateResponse* pOutputStateResponse = (struct tMessages::tOutputStateResponse*)pData;
	struct tMessages::tOverviewStateResponse *pOverviewStateResponse = (struct tMessages::tOverviewStateResponse*)pData;

	if (type == tMessages::MessageType_ExternalEvent)
	{
		switch (data)
		{
		case tMessages::ExternalEvent_VersionResponse:
			cmd.print(F("FW Version for device "));
			cmd.print(pVersionResponse->SenderID);
			cmd.print(F("="));
			cmd.print(pVersionResponse->Major);
			cmd.print(F("."));
			cmd.print(pVersionResponse->Minor);
			cmd.print(F("."));
			cmd.println(pVersionResponse->Patch);

			break;

		case tMessages::ExternalEvent_OutputStateResponse:
			cmd.print(F("PowerState for device "));
			cmd.print(pOutputStateResponse->SenderID);
			cmd.print(F(" output ID "));
			cmd.print(pOutputStateResponse->OutputID);
			cmd.print(F("="));
			cmd.print(pOutputStateResponse->PowerState);
			cmd.print(F(" with timers = "));
			cmd.print(pOutputStateResponse->TimerValue);
			cmd.print(F(" default timer = "));
			cmd.println(pOutputStateResponse->DefaultTimer);

			break;

		case tMessages::ExternalEvent_OverviewStateResponse:
			cmd.print(F("PowerStateBitmap for device "));
//			LOG(print(SenderID,HEX));
			cmd.print(F("="));
//			LOG(print(PowerState,BIN));
			cmd.print(F(" with timers map="));
//			LOG(println(TimerState,BIN));

			break;
		}
	}
}

bool TelnetEnableLogs(Commander &Cmdr)
{
   if (pTelnetSession) pTelnetSession->EnableLogs();
   return true;
}

bool  TelnetDisableLogs(Commander &Cmdr)
{
   if (pTelnetSession) pTelnetSession->DisableLogs();
   return true;
}

#if CONFIG_TLE8457_COMM_LIB

bool send_GetVersion(Commander &Cmdr)
{
  int Dst;
  if(Cmdr.getInt(Dst))
  {
	  tOutgoingFrames::SendMsgVersionRequest(Dst);
  }
  else
  {
    Cmdr.println(F("Usage: GetVersion dst_dev_id"));
    return false;
  }

  return true;
}

bool send_Reset(Commander &Cmdr)
{
  int Dst;
  if(Cmdr.getInt(Dst))
  {
	  tOutgoingFrames::SendMsgReset(Dst);
  }
  else
  {
    Cmdr.println(F("Usage: SendReset dst_dev_id"));
    return false;
  }

  return true;
}

#if CONFIG_OUTPUT_PROCESS
bool send_stateOverviewHandler(Commander &Cmdr)
{

  int Dst;
  if(Cmdr.getInt(Dst))
  {
	  tOutgoingFrames::SendMsgOverviewStateRequest(Dst);
  }
  else
  {
    Cmdr.println(F("Usage: StateOverview dst_dev_id"));
    return false;
  }

  return true;
}

bool send_OutputStateHandler(Commander &Cmdr)
{

  int Dst;
  int OutId;
  if(!Cmdr.getInt(Dst))
  {
    goto error;
  }
  if (! Cmdr.getInt(OutId))
  {
    goto error;
  }

  tOutgoingFrames::SendMsgOutputStateRequest(Dst,OutId);

  return true;
error:
  Cmdr.println(F("Usage: OutputState dst_dev_id output_id"));
  return false;
}

bool send_SetOutputHandler(Commander &Cmdr)
{
  int Dst;
  int OutId;
  int State;
  int Timer = DEFAULT_TIMER;

  if(!Cmdr.getInt(Dst))
  {
    goto error;
  }
  if (! Cmdr.getInt(OutId))
  {
    goto error;
  }
  if (! Cmdr.getInt(State))
  {
    goto error;
  }
  if (! Cmdr.getInt(Timer))
  {
    //goto finish;
  }

  tOutgoingFrames::SendMsgSetOutput(Dst, OutId, State, Timer);
  return true;
error:
  Cmdr.println(F("Usage: SetOutput dst_dev_id output_id state[0/1] [timer[sec]]"));
  return false;
}
#endif // CONFIG_OUTPUT_PROCESS
#endif // CONFIG_TLE8457_COMM_LIB

#endif // CONFIG_TELNET_SERVER
