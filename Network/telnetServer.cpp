#include "telnetServer.h"


/*
 * global telnet commands
 *
 * to add app specific commands, define specific macros, like below
 * TELNET_APP_SPECIFIC_INCLUDE must be in globally included file
 * TELNET_APP_SPECIFIC_COMMANDS_CODE should be in file included by TELNET_APP_SPECIFIC_INCLUDE
 *
 * note: there may be only 1 file extra included. If you need more, you should include them from  it
 *
 *
 *#define TELNET_APP_SPECIFIC_INCLUDE "../../../telnet_app_specific.h"
 *
 *
 *#if CONFIG_OPTION1
 *   #define TELNET_APP_SPECIFIC_COMMANDS_CODE \
 *        {"option1A",      TelnetOption1A,             "option1A"},   \
 *        {"option1B",      TelnetOption1B,             "option1B"},
 *#endif
 *
 *#if CONFIG_OPTION2
 *   #define TELNET_APP_SPECIFIC_COMMANDS_CODE1 \
 *        {"option2A",      TelnetOption2A,             "option2A"},   \
 *        {"option2B",      TelnetOption2B,             "option2B"},
 *#endif
 *
 * ...
 */

#include "../../../global.h"
#if CONFIG_TELNET_SERVER

#ifdef TELNET_APP_SPECIFIC_INCLUDE
#include TELNET_APP_SPECIFIC_INCLUDE
#endif // TELNET_APP_SPECIFIC_INCLUDE

#include "../TLE8457_serial/TLE8457_serial_lib.h"
#include "../TLE8457_serial/tOutgoingFrames.h"

#if CONFIG_OUTPUT_PROCESS
#include "../tOutputProcess.h"
#endif //CONFIG_OUTPUT_PROCESS

#if CONFIG_NODE_SCAN_TASK
#include "../TLE8457_serial/NodeScanTask.h"
#endif CONFIG_NODE_SCAN_TASK

#if CONFIG_TELNET_COMMANDS_SENSORS
#include "../sensors/tSensor.h"
#endif //CONFIG_TELNET_COMMANDS_SENSORS
// must be static-global (why? - only 1 telnet session may be active)
Commander cmd;

tTelnetSession *pTelnetSession = NULL;

tTelnetSession::tTelnetSession(EthernetClient aEthernetClient,commandList_t *pTelnetCommands, uint8_t NumOfTelnetCommands)
   : tTcpSession(aEthernetClient, TELNET_SESSION_TIMEOUT), tLogTransport()
{
  DEBUG_PRINTLN_2("TELNET Session started");
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

static bool TelnetEnableLogs(Commander &Cmdr)
{
   if (pTelnetSession) pTelnetSession->EnableLogs();
   return true;
}

static bool TelnetDisableLogs(Commander &Cmdr)
{
   if (pTelnetSession) pTelnetSession->DisableLogs();
   return true;
}

#if CONFIG_TLE8457_COMM_LIB

static bool send_GetVersion_remote(Commander &Cmdr)
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

static bool send_Reset_remote(Commander &Cmdr)
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

#else // CONFIG_TLE8457_COMM_LIB

static bool send_GetVersion_local(Commander &Cmdr)
{
    Cmdr.print(F("Version is "));
    Cmdr.println(F(FW_VERSION));
    return true;
}

static bool send_Reset_local(Commander &Cmdr)
{
    cli();
    while(1); // let watchdog reboot the device
}

#endif //CONFIG_TLE8457_COMM_LIB

#if CONFIG_TLE8457_COMM_LIB
#if CONFIG_OUTPUT_PROCESS
static bool send_stateOverviewHandler(Commander &Cmdr)
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

static bool send_OutputStateHandler(Commander &Cmdr)
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

static bool send_SetOutputHandler(Commander &Cmdr)
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

#if CONFIG_NODE_SCAN_TASK
static bool trigger_ScanNodes(Commander &Cmdr)
{
   NodeScanTask::trigger();
}
#endif //CONFIG_NODE_SCAN_TASK

#if CONFIG_TELNET_COMMANDS_SENSORS
static bool send_GetSensorByIdReqestHandler(Commander &Cmdr)
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
    DEBUG_PRINTLN_2("SENDING MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST");
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST, sizeof(Message), &Message);

    return true;
  error:
    Cmdr.println(F("Usage: GetSensorById sensor_id [dst_dev = broadcast]"));
    return false;
}

static bool send_GetSensorMeasurementReqest(Commander &Cmdr)
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
    DEBUG_PRINTLN_2("SENDING MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST");
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST, sizeof(Message), &Message);

    return true;
  error:
    Cmdr.println(F("Usage: GetSensorMeasurement sensor_id [dst_dev = broadcast]"));
    return false;
}
#endif CONFIG_TELNET_COMMANDS_SENSORS

#if CONFIG_TELNET_COMMANDS_SENSORS_REMOTE_CONTROL
static bool send_CreateSensorRequest(Commander &Cmdr)
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
    DEBUG_PRINTLN_2("SENDING MESSAGE_TYPE_SENSOR_CREATE");
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_CREATE, sizeof(Msg), &Msg);
    return true;
  error:
    Cmdr.println(F("Usage: CreateSensor dev_id sensor_type sensor_id"));
    return false;
}

static bool send_ConfigureSensorRequest(Commander &Cmdr)
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

static bool send_StartSensorRequest(Commander &Cmdr)
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

static bool send_StopSensorRequest(Commander &Cmdr)
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
#endif CONFIG_TELNET_COMMANDS_SENSORS_REMOTE_CONTROL

#if CONFIG_TELNET_COMMANDS_SENSORS_EEPROM_CONTROL
static bool send_saveSensorsToEeprom(Commander &Cmdr)
{
	int Dst;
    if(!Cmdr.getInt(Dst))
    {
      goto error;
    }

    DEBUG_PRINTLN_2("SENDING MESSAGE_TYPE_SENSOR_SAVE");
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_SAVE, 0, NULL);
    return true;

    error:
      Cmdr.println(F("Usage: SaveSensorsToEeprom dst_dev"));
      return false;
}

static bool send_restoreSensorsFromEeprom(Commander &Cmdr)
{
	int Dst;
    if(!Cmdr.getInt(Dst))
    {
      goto error;
    }

    DEBUG_PRINTLN_2("SENDING MESSAGE_TYPE_SENSOR_RESTORE");
    CommSenderProcess::Instance->Enqueue(Dst, MESSAGE_TYPE_SENSOR_RESTORE, 0, NULL);
    return true;

    error:
      Cmdr.println(F("Usage: RestoreSensorsFromEeprom dst_dev"));
      return false;
}
#endif CONFIG_TELNET_COMMANDS_SENSORS_EEPROM_CONTROL

#endif // CONFIG_TLE8457_COMM_LIB

const commandList_t TelnetCommands[] = {
  {"enableLogs",      TelnetEnableLogs,             "enable logs on telnet console"},
  {"disableLogs",     TelnetDisableLogs,            "disables logs on telnet console"},
#if CONFIG_TLE8457_COMM_LIB
  {"GetVersion",      send_GetVersion_remote,              "show version"},
  {"Reset",           send_Reset_remote,                   "reset the system"},
#else // CONFIG_TLE8457_COMM_LIB
  {"GetVersion",      send_GetVersion_local,              "show version"},
  {"Reset",           send_Reset_local,                   "reset the system"},
#endif // CONFIG_TLE8457_COMM_LIB


  #if CONFIG_NODE_SCAN_TASK
  {"ScanActiveNodes", trigger_ScanNodes,            "Scan the bus for nodes from 1 to 32"},
#endif //CONFIG_NODE_SCAN_TASK
#if CONFIG_TLE8457_COMM_LIB
#if CONFIG_OUTPUT_PROCESS
  {"StateOverview",   send_stateOverviewHandler,    "StateOverview dev_id"},
  {"OutputState",     send_OutputStateHandler,      "OutputState dev_id out_id"},
  {"SetOutput",       send_SetOutputHandler,        "SetOutput dev_id out_id state [timer] (not set=default, 0-forever)"},
#endif // CONFIG_OUTPUT_PROCESS
#if CONFIG_TELNET_COMMANDS_SENSORS
  {"GetSensorById",   send_GetSensorByIdReqestHandler,"GetSensorById sensor_id [dst_dev = broadcast]"},
  {"GetSensorMeasurement",  send_GetSensorMeasurementReqest, "GetSensorMeasurement sensor_id [dst_dev = broadcast]"},
#if CONFIG_TELNET_COMMANDS_SENSORS_REMOTE_CONTROL
  {"CreateSensor", send_CreateSensorRequest, "CreateSensor dev_id sensor_type sensor_id"},
  {"StartSensor", send_StartSensorRequest, "StartSensor sensor_id [sensor_ev_mask = EV_TYPE_MEASUREMENT_COMPLETED] [dev_id = broadcast]"},
  {"StopSensor", send_StopSensorRequest, "StopSensor sensor_id [dev_id = broadcast]"},
  {"ConfigureSensor", send_ConfigureSensorRequest, "ConfigureSensor sensor_id period [dev_id = broadcast]"},
#endif //CONFIG_TELNET_COMMANDS_SENSORS_REMOTE_CONTROL
#if CONFIG_TELNET_COMMANDS_SENSORS_EEPROM_CONTROL
  {"SaveSensorsToEeprom", send_saveSensorsToEeprom, "SaveSensorsToEeprom dst_dev"},
  {"RestoreSensorsFromEeprom", send_restoreSensorsFromEeprom, "RestoreSensorsFromEeprom dst_dev"},
#endif CONFIG_TELNET_COMMANDS_SENSORS_EEPROM_CONTROL
#endif // CONFIG_TELNET_COMMANDS_SENSORS
#endif //CONFIG_TLE8457_COMM_LIB

#ifdef TELNET_APP_SPECIFIC_COMMANDS_CODE
  TELNET_APP_SPECIFIC_COMMANDS_CODE
#endif //TELNET_APP_SPECIFIC_COMMANDS_CODE

#ifdef TELNET_APP_SPECIFIC_COMMANDS_CODE1
  TELNET_APP_SPECIFIC_COMMANDS_CODE1
#endif //TELNET_APP_SPECIFIC_COMMANDS_CODE1

#ifdef TELNET_APP_SPECIFIC_COMMANDS_CODE2
  TELNET_APP_SPECIFIC_COMMANDS_CODE2
#endif //TELNET_APP_SPECIFIC_COMMANDS_CODE2

#ifdef TELNET_APP_SPECIFIC_COMMANDS_CODE3
  TELNET_APP_SPECIFIC_COMMANDS_CODE3
#endif //TELNET_APP_SPECIFIC_COMMANDS_CODE3

#ifdef TELNET_APP_SPECIFIC_COMMANDS_CODE4
  TELNET_APP_SPECIFIC_COMMANDS_CODE4
#endif //TELNET_APP_SPECIFIC_COMMANDS_CODE4

#ifdef TELNET_APP_SPECIFIC_COMMANDS_CODE5
  TELNET_APP_SPECIFIC_COMMANDS_CODE5
#endif //TELNET_APP_SPECIFIC_COMMANDS_CODE5

};

tTelnetServer TelnetServer(TelnetCommands,sizeof(TelnetCommands));

#endif // CONFIG_TELNET_SERVER
