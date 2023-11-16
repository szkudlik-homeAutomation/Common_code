#include "telnetServer.h"

#include "../../../global.h"
#if CONFIG_TELNET_SERVER
#include "../TLE8457_serial/tOutgoingFrames.h"
#include "../TLE8457_serial/TLE8457_serial_lib.h"

#if CONFIG_OUTPUT_PROCESS
#include "../tOutputProcess.h"
#include "../TLE8457_serial/NodeScanTask.h"
#endif //CONFIG_OUTPUT_PROCESS

// must be static-global (why? - only 1 telnet session may be active)
Commander cmd;

tTelnetSession *pTelnetSession = NULL;

tTelnetSession::tTelnetSession(EthernetClient aEthernetClient,commandList_t *pTelnetCommands, uint8_t NumOfTelnetCommands)
   : tTcpSession(aEthernetClient, TELNET_SESSION_TIMEOUT), tLogTransport()
{
  DEBUG_PRINTLN_3("TELNET Session started");
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

#if CONFIG_NODE_SCAN_TASK
bool trigger_ScanNodes(Commander &Cmdr)
{
   NodeScanTask::trigger();
}
#endif //CONFIG_NODE_SCAN_TASK

#endif // CONFIG_OUTPUT_PROCESS

#if CONFIG_SENSORS
bool send_GetSensorByIdReqestHandler(Commander &Cmdr)
{
    int Dst = DEVICE_ID_BROADCAST;
    int SensorId;

    if(!Cmdr.getInt(SensorId))
    {
      goto error;
    }
    Cmdr.getInt(Dst);

    tOutgoingFrames::SendGetSensorByIdReqest(Dst, SensorId);
    return true;
  error:
    Cmdr.println(F("Usage: GetSensorById sensor_id [dst_dev]   if dst is not provided => broadcast"));
    return false;
}

bool send_GetSensorMeasurementReqest(Commander &Cmdr)
{
    int Dst = DEVICE_ID_BROADCAST;
    int SensorId;

    if(!Cmdr.getInt(SensorId))
    {
      goto error;
    }
    Cmdr.getInt(Dst);

    tOutgoingFrames::SendGetSensorMeasurementReqest(Dst, SensorId);
    return true;
  error:
    Cmdr.println(F("Usage: GetSensorMeasurement sensor_id [dst_dev]   if dst is not provided => broadcast"));
    return false;
}

bool send_CreateSensorRequest(Commander &Cmdr)
{
    int Dst;
    int SensorType;
    int SensorId;

    if(!Cmdr.getInt(Dst))
    {
      goto error;
    }
    if(!Cmdr.getInt(SensorType))
    {
      goto error;
    }
    if(!Cmdr.getInt(SensorId))
    {
      goto error;
    }

    tOutgoingFrames::SendSensorCreate(Dst, SensorType, SensorId);
    return true;
  error:
    Cmdr.println(F("Usage: CreateSensor dev_id sensor_type sensor_id"));
    return false;
}

bool send_StartSensorRequest(Commander &Cmdr)
{
    int Dst;
    int SensorId;
    tSensor::tEventMask SensorEventMask;
    SensorEventMask.MeasurementCompleted = 1;

    if(!Cmdr.getInt(Dst))
    {
      goto error;
    }
    if(!Cmdr.getInt(SensorId))
    {
      goto error;
    }

    Cmdr.getInt(SensorEventMask.Byte);

    tOutgoingFrames::SendSensorStart(Dst, SensorId, SensorEventMask);
    return true;
  error:
    Cmdr.println(F("Usage: StartSensor dev_id sensor_id [sensor_ev_mask = EV_TYPE_MEASUREMENT_COMPLETED]"));
    return false;
}

bool send_StopSensorRequest(Commander &Cmdr)
{
    int Dst;
    int SensorId;

    if(!Cmdr.getInt(Dst))
    {
      goto error;
    }

    if(!Cmdr.getInt(SensorId))
    {
      goto error;
    }

    tOutgoingFrames::SendSensorStop(Dst, SensorId);
    return true;

 error:
    Cmdr.println(F("Usage: StopSensor dev_id sensor_id"));
    return false;
}


#endif //CONFIG_SENSORS

#endif // CONFIG_TLE8457_COMM_LIB


#endif // CONFIG_TELNET_SERVER
