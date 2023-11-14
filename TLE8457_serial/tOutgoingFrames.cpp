/*
 * tOutgoingFrames.cpp
 *
 *  Created on: Oct 10, 2023
 *      Author: mszkudli
 */

#include "../../../global.h"
#if CONFIG_TLE8457_COMM_LIB

#include "tOutgoingFrames.h"
#include "TLE8457_serial_lib.h"

// VERSION HANDSHAKE
bool tOutgoingFrames::SendMsgVersionRequest(uint8_t RecieverID)
{
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_FW_VERSION_REQUEST");
  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_FW_VERSION_REQUEST,0,NULL);
  return true;
}

bool tOutgoingFrames::SendMsgVersionResponse(uint8_t RecieverID, uint8_t Major, uint8_t Minor, uint8_t Patch)
{
  tMessageTypeFwVesionResponse Msg;
  Msg.Major = Major;
  Msg.Minor = Minor;
  Msg.Patch = Patch;

  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_FW_VERSION_RESPONSE,sizeof(Msg),&Msg);
};

bool tOutgoingFrames::SendMsgReset(uint8_t RecieverID)
{
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_FORCE_RESET");
  CommSender.Enqueue(RecieverID,MESSAGE_TYPE_FORCE_RESET,0,NULL);
}

bool tOutgoingFrames::SendMsgStatus(uint8_t RecieverID, uint8_t Status)
{
    tMesssageGeneralStatus Msg;
    DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_GENERAL_STATUS");
    Msg.Status = Status;
    CommSender.Enqueue(RecieverID, MESSAGE_TYPE_GENERAL_STATUS, sizeof(Msg), &Msg);
}

#if CONFIG_OUTPUT_PROCESS
// OVERVIEW STATE HANDSHAKE
bool tOutgoingFrames::SendMsgOverviewStateRequest(uint8_t RecieverID)
{
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_OVERVIEW_STATE_REQUEST");
  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_OVERVIEW_STATE_REQUEST, 0, NULL);
  return true;
}

bool tOutgoingFrames::SendMsgOverviewStateResponse(uint8_t RecieverID, uint8_t  PowerState, uint8_t  TimerState)
{
  tMessageTypeOverviewStateResponse Msg;
  Msg.PowerState = PowerState;
  Msg.TimerState = TimerState;
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE");
  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE, sizeof(Msg), &Msg);
  return true;
}

// GET OUTPUT STATE HANDSHAKE
bool tOutgoingFrames::SendMsgOutputStateRequest(uint8_t RecieverID, uint8_t  OutputID)
{
  tMessageTypeOutputStateRequest Msg;
  Msg.OutputID = OutputID;
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_OUTPUT_STATE_REQUEST");
  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_OUTPUT_STATE_REQUEST, sizeof(Msg), &Msg);
  return true;
};

bool tOutgoingFrames::SendMsgOutputStateResponse(uint8_t RecieverID, uint8_t  OutputID, uint8_t  PowerState, uint16_t TimerValue, uint16_t DefaultTimer)
{
  tMessageTypeOutputStateResponse Msg;
  Msg.OutputID = OutputID;
  Msg.PowerState = PowerState;
  Msg.TimerValue = TimerValue;
  Msg.DefaultTimer = DefaultTimer;
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_OUTPUT_STATE_RESPONSE");
  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_OUTPUT_STATE_RESPONSE, sizeof(Msg), &Msg);

  return true;
};

// SET OUTPUT
bool tOutgoingFrames::SendMsgSetOutput(uint8_t RecieverID, uint8_t  OutId, uint8_t  State, uint16_t Timer)
{
  tMessageTypeSetOutput Message;
  Message.OutId = OutId;
  Message.State = State;
  Message.Timer = Timer;
  DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_SET_OUTPUT");
  CommSender.Enqueue(RecieverID, MESSAGE_TYPE_SET_OUTPUT, sizeof(Message), &Message);
  return true;
}

#endif // CONFIG_OUTPUT_PROCESS

#if CONFIG_SENSORS
bool tOutgoingFrames::SendGetSensorByIdReqest(uint8_t RecieverID, uint8_t SensorID)
{
    tMessageGetSensorByIdReqest Message;
    Message.SensorID = SensorID;
    DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST");
    CommSender.Enqueue(RecieverID, MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST, sizeof(Message), &Message);
}

bool tOutgoingFrames::SendGetSensorByIdResponse(uint8_t RecieverID, tMessageGetSensorByIdResponse *pMessage)
{
    DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE");
    CommSender.Enqueue(RecieverID, MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE, sizeof(*pMessage), pMessage);
}

static bool tOutgoingFrames::SendGetSensorMeasurementReqest(uint8_t RecieverID, uint8_t SensorID)
{
    DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST");
    tMessageGetSensorMeasurementReqest Message;
    Message.SensorID = SensorID;
    CommSender.Enqueue(RecieverID, MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST, sizeof(Message), &Message);
}

static bool tOutgoingFrames::SendSensorEvent(uint8_t RecieverID, uint8_t SensorID, uint8_t EventType, bool onDemand,
		void *pPayload, uint8_t payloadSize, uint8_t seq, bool LastSegment)
{
    DEBUG_PRINTLN_3("===================>sending MESSAGE_TYPE_SENSOR_EVENT");
    tMessageSensorEvent Message;
    if (payloadSize > SENSOR_MEASUREMENT_PAYLOAD_SIZE)
    {
        DEBUG_PRINTLN_3("SendSensorEvent - payload too big");
        return false;
    }
    Message.Header.LastSegment = LastSegment;
    Message.Header.onDemand = onDemand;
    Message.Header.EventType = EventType;
    Message.Header.SegmentSeq = seq;
    Message.Header.SensorID = SensorID;
    memset(Message.Payload, 0, SENSOR_MEASUREMENT_PAYLOAD_SIZE);
    memcpy(Message.Payload, pPayload, payloadSize);

    CommSender.Enqueue(RecieverID, MESSAGE_TYPE_SENSOR_EVENT, sizeof(tMessageSensorEvent), &Message);
}


#endif //CONFIG_SENSORS

#endif // CONFIG_TLE8457_COMM_LIB
