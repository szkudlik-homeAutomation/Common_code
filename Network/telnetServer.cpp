#include "telnetServer.h"

#include "../../../global.h"
#if CONFIG_TELNET_SERVER
#include "../TLE8457_serial/TLE8457_serial_lib.h"
#include "../TLE8457_serial/tOutgoingFrames.h"

#if CONFIG_OUTPUT_PROCESS
#include "../tOutputProcess.h"
#include "../TLE8457_serial/NodeScanTask.h"
#endif //CONFIG_OUTPUT_PROCESS
#if CONFIG_TELNET_COMMANDS_SENSORS
#include "../sensors/tSensor.h"
#endif //CONFIG_TELNET_COMMANDS_SENSORS
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

#if CONFIG_TELNET_COMMANDS_SENSORS
bool send_GetSensorByIdReqestHandler(Commander &Cmdr)
{
    int Dst = DEVICE_ID_BROADCAST;
    int SensorId;

    if(!Cmdr.getInt(SensorId))
    {
      goto error;
    }
    Cmdr.getInt(Dst);

    tMessageGetSensorByIdReqest Message;
    Message.SensorID = SensorId;
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST, sizeof(Message), &Message);

    return true;
  error:
    Cmdr.println(F("Usage: GetSensorById sensor_id [dst_dev = broadcast]"));
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


    tMessageSensorCreate Msg;
    Msg.SensorID = SensorId;
    Msg.SensorType = SensorType;
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_CREATE, sizeof(Msg), &Msg);
    return true;
  error:
    Cmdr.println(F("Usage: CreateSensor dev_id sensor_type sensor_id"));
    return false;
}

bool send_ConfigureSensorRequest(Commander &Cmdr)
{
    int Dst = DEVICE_ID_BROADCAST;
    int SensorId;
    int period;

    if(!Cmdr.getInt(SensorId))
    {
      goto error;
    }

    if(!Cmdr.getInt(period))
    {
      goto error;
    }

    Cmdr.getInt(Dst);

    uint8_t seq = 0;
    // TODO - payload data
    tOutgoingFrames::SendSensorConfigure(Dst, SensorId, seq, true /* last segment */, NULL /*payload */, 0 /* payload size */, period);

    return true;

 error:
    Cmdr.println(F("Usage: ConfigureSensor sensor_id period [dev_id = broadcast]"));
    return false;
}

bool send_StartSensorRequest(Commander &Cmdr)
{
    int Dst = DEVICE_ID_BROADCAST;
    int SensorId;
    uint8_t SensorEventMask;
    SensorEventMask = 1 << EV_TYPE_MEASUREMENT_COMPLETED;

    if(!Cmdr.getInt(SensorId))
    {
      goto error;
    }

    Cmdr.getInt(SensorEventMask);
    Cmdr.getInt(Dst);

    tMessageSensorStart Msg;
    Msg.SensorID = SensorId;
    Msg.SensorEventMask = SensorEventMask;
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_START, sizeof(Msg), &Msg);

    return true;
  error:
    Cmdr.println(F("Usage: StartSensor sensor_id [sensor_ev_mask = EV_TYPE_MEASUREMENT_COMPLETED] [dev_id = broadcast]"));
    return false;
}

bool send_StopSensorRequest(Commander &Cmdr)
{
    int Dst = DEVICE_ID_BROADCAST;
    int SensorId;

    if(!Cmdr.getInt(SensorId))
    {
      goto error;
    }

    Cmdr.getInt(Dst);

    tMessageSensorStop Msg;
    Msg.SensorID = SensorId;
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_STOP, sizeof(Msg), &Msg);
    return true;

 error:
    Cmdr.println(F("Usage: StopSensor sensor_id [dev_id = broadcast]"));
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

    tMessageGetSensorMeasurementReqest Message;
    Message.SensorID = SensorId;
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST, sizeof(Message), &Message);

    return true;
  error:
    Cmdr.println(F("Usage: GetSensorMeasurement sensor_id [dst_dev = broadcast]"));
    return false;
}

bool send_saveSensorsToEeprom(Commander &Cmdr)
{
	int Dst;
    if(!Cmdr.getInt(Dst))
    {
      goto error;
    }

    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_SAVE, 0, NULL);
    return true;

    error:
      Cmdr.println(F("Usage: SaveSensorsToEeprom dst_dev"));
      return false;
}

bool send_restoreSensorsFromEeprom(Commander &Cmdr)
{
	int Dst;
    if(!Cmdr.getInt(Dst))
    {
      goto error;
    }

    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_RESTORE, 0, NULL);
    return true;

    error:
      Cmdr.println(F("Usage: RestoreSensorsFromEeprom dst_dev"));
      return false;
}

#endif // CONFIG_TELNET_COMMANDS_SENSORS
#endif // CONFIG_TLE8457_COMM_LIB

#endif // CONFIG_TELNET_SERVER
