/*
 * tIncomingFrameHanlder.cpp
 *
 *  Created on: Oct 10, 2023
 *      Author: mszkudli
 */

#include "../../../global.h"
#if CONFIG_TLE8457_COMM_LIB

#include "tIncomingFrameHanlder.h"
#include "tOutgoingFrames.h"

#if CONFIG_OUTPUT_PROCESS
#include "../tOutputProcess.h"
#endif // CONFIG_OUTPUT_PROCESS

#if CONFIG_SENSORS
#include "../sensors/tSensor.h"
#endif //CONFIG_SENSORS


uint8_t tIncomingFrameHanlder::handleCommonMessages(uint16_t data, void *pData)
{
    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;
    uint8_t status = STATUS_SUCCESS;

    switch (data)   // messageType
    {
       case MESSAGE_TYPE_FW_VERSION_REQUEST:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FW_VERSION_REQUEST");
             HandleMsgVersionRequest(SenderDevId);
           break;

       case MESSAGE_TYPE_FW_VERSION_RESPONSE:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FW_VERSION_RESPONSE");
             HandleMsgVersionResponse(SenderDevId,(tMessageTypeFwVesionResponse*)(pFrame->Data));
           break;

       case MESSAGE_TYPE_FORCE_RESET:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_FORCE_RESET");
             cli();
             while(1); // let watchdog reboot the device
           break;
       case MESSAGE_TYPE_GENERAL_STATUS:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_GENERAL_STATUS");
           HandleMsgGeneralStatus(SenderDevId, (tMesssageGeneralStatus *)(pFrame->Data));
           break;

#if CONFIG_OUTPUT_PROCESS
       case MESSAGE_TYPE_OVERVIEW_STATE_REQUEST:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OVERVIEW_STATE_REQUEST");
           HandleMsgOverviewStateRequest(SenderDevId);
           break;

       case MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE");
           HandleMsgOverviewStateResponse(SenderDevId,(tMessageTypeOverviewStateResponse*) (pFrame->Data));
           break;

       case MESSAGE_TYPE_OUTPUT_STATE_REQUEST:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OUTPUT_STATE_REQUEST");
           HandleMsgOutputStateRequest(SenderDevId,(tMessageTypeOutputStateRequest*)(pFrame->Data));
           break;

       case MESSAGE_TYPE_OUTPUT_STATE_RESPONSE:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_OUTPUT_STATE_RESPONSE");
           HandleMsgOutputStateResponse(SenderDevId,(tMessageTypeOutputStateResponse*) (pFrame->Data));
           break;

       case MESSAGE_TYPE_SET_OUTPUT:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_SET_OUTPUT");
           HandleMsgSetOutput(SenderDevId,(tMessageTypeSetOutput*)(pFrame->Data));
           break;
#endif //CONFIG_OUTPUT_PROCESS
#if CONFIG_SENSORS
       case MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST");
           HandleMsgGetSensorByIdReqest(SenderDevId, (tMessageGetSensorByIdReqest*)(pFrame->Data));
           break;

       case MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE");
           HandleMsgGetSensorByIdResponse(SenderDevId, (tMessageGetSensorByIdResponse*)(pFrame->Data));
           break;

       case MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST");
           HandleMsgGetSensorMeasurementReqest(SenderDevId,(tMessageGetSensorMeasurementReqest*)(pFrame->Data));
           break;

       case MESSAGE_TYPE_SENSOR_EVENT:
           DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_SENSOR_EVENT");
           HandleMsgSensorEvent(SenderDevId,(tMessageSensorEvent*)(pFrame->Data));
           break;

#endif //CONFIG_SENSORS

       default:
           status = STATUS_UNKNOWN_MESSAGE;
    }

    return status;
}

void tIncomingFrameHanlder::HandleMsgVersionRequest(uint8_t SenderID)
{
    tOutgoingFrames::SendMsgVersionResponse(SenderID,FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH);
}

void tIncomingFrameHanlder::HandleMsgVersionResponse(uint8_t SenderID, tMessageTypeFwVesionResponse *pMessage)
{
	tMessages::tVersionResponse VersionResponse;

	VersionResponse.SenderID = SenderID;
	VersionResponse.Major = pMessage->Major;
	VersionResponse.Minor = pMessage->Minor;
	VersionResponse.Patch = pMessage->Patch;

	LOG_PRINT("FW Version for device ");
	LOG(print(SenderID,HEX));
	LOG_PRINT("=");
	LOG(print(pMessage->Major,DEC));
	LOG_PRINT(".");
	LOG(print(pMessage->Minor,DEC));
	LOG_PRINT(".");
	LOG(println(pMessage->Patch,DEC));

	tMessageReciever::Dispatch(tMessages::MessageType_ExternalEvent,tMessages::ExternalEvent_VersionResponse,&VersionResponse);
}

void tIncomingFrameHanlder::HandleMsgGeneralStatus(uint8_t SenderID, tMesssageGeneralStatus *pMessage)
{
    LOG_PRINT("Status from node: ");
    LOG(print(SenderID,HEX));
    LOG_PRINT(" ");
    LOG(println(pMessage->Status,DEC));
    //TODO: send a message
}

#if CONFIG_OUTPUT_PROCESS
void tIncomingFrameHanlder::HandleMsgOverviewStateRequest(uint8_t SenderID)
{
   tOutgoingFrames::SendMsgOverviewStateResponse(SenderID,tOutputProcess::get()->GetOutputStateMap(),tOutputProcess::get()->GetOutputTimersStateMap());
}

void tIncomingFrameHanlder::HandleMsgOverviewStateResponse(uint8_t SenderID, tMessageTypeOverviewStateResponse* Message)
{
	LOG_PRINT("PowerStateBitmap for device ");
	LOG(print(SenderID,HEX));
	LOG_PRINT("=");
	LOG(print(Message->PowerState,BIN));
	LOG_PRINT(" with timers map=");
	LOG(println(Message->TimerState,BIN));
   //TODO: send a message
}

void tIncomingFrameHanlder::HandleMsgOutputStateRequest(uint8_t SenderID, tMessageTypeOutputStateRequest* Message)
{
  if (Message->OutputID < NUM_OF_OUTPUTS)
  {
      uint16_t DefTimer;
#if CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER
      EEPROM.get(EEPROM_DEFAULT_TIMER_VALUE_OFFSET+Message->OutputID*(sizeof(uint16_t)),DefTimer);
#else CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER
      DefTimer = 0;
#endif CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER

      tOutgoingFrames::SendMsgOutputStateResponse(SenderID,Message->OutputID, tOutputProcess::get()->GetOutputState(Message->OutputID), tOutputProcess::get()->GetOutputTimer(Message->OutputID),DefTimer);
  }
}

void tIncomingFrameHanlder::HandleMsgOutputStateResponse(uint8_t SenderID, tMessageTypeOutputStateResponse* Message)
{
	tMessages::tOutputStateResponse OutputStateResponse;
	OutputStateResponse.SenderID = SenderID;
	OutputStateResponse.OutputID = Message->OutputID;
	OutputStateResponse.PowerState = Message->PowerState;
	OutputStateResponse.TimerValue = Message->TimerValue;
	OutputStateResponse.DefaultTimer = Message->DefaultTimer;

	LOG_PRINT("PowerState for device ");
	LOG(print(OutputStateResponse.SenderID,HEX));
	LOG_PRINT(" output ID ");
	LOG(print(OutputStateResponse.OutputID,DEC));
	LOG_PRINT("=");
	LOG(print(OutputStateResponse.PowerState,DEC));
	LOG_PRINT(" with timers = ");
	LOG(print(OutputStateResponse.TimerValue,DEC));
    LOG_PRINT(" default timer = ");
    LOG(println(OutputStateResponse.DefaultTimer,DEC));

    tMessageReciever::Dispatch(tMessages::MessageType_ExternalEvent,tMessages::ExternalEvent_OutputStateResponse,&OutputStateResponse);
}

void tIncomingFrameHanlder::HandleMsgSetOutput(uint8_t SenderID, tMessageTypeSetOutput* Message)
{
   if (Message->OutId >= NUM_OF_OUTPUTS)
   {
     // drop it
     return;
   }
   uint16_t Timer = Message->Timer;

#if CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER
   if (DEFAULT_TIMER == Timer)
   {
     EEPROM.get(EEPROM_DEFAULT_TIMER_VALUE_OFFSET+Message->OutId*(sizeof(uint16_t)),Timer);
   }
#endif // CONFIG_OUTPUT_USE_EEPROM_DEF_TIMER

  tOutputProcess::get()->SetOutput(Message->OutId,Message->State,Timer,tOutputProcess::ForceTimer);
}

#endif // CONFIG_OUTPUT_PROCESS

#if CONFIG_SENSORS
void tIncomingFrameHanlder::HandleMsgGetSensorByIdReqest(uint8_t SenderID, tMessageGetSensorByIdReqest *Message)
{
	tSensor *pSensor = tSensor::getSensor(Message->SensorID);
	if (NULL != pSensor)
	{
		tMessageGetSensorByIdResponse Response;
		Response.SensorID = Message->SensorID;
		Response.MeasurementPeriod = pSensor->GetMeasurementPeriod();
		Response.ApiVersion = pSensor->getSensorApiVersion();
		Response.SensorType = pSensor->getSensorType();
		Response.isConfigured = pSensor->isConfigured();
		Response.isMeasurementValid = pSensor->isMeasurementValid();
		Response.isRunning = pSensor->isRunning();
		Response.EventsMask = pSensor->getSensorSerialEventsMask();
	    tOutgoingFrames::SendGetSensorByIdResponse(SenderID, &Response);
	}
}

void tIncomingFrameHanlder::HandleMsgGetSensorByIdResponse(uint8_t SenderID, tMessageGetSensorByIdResponse *Message)
{
	LOG_PRINT("Sensor ID ");
	LOG(print(Message->SensorID, DEC));
	LOG_PRINT(" found on node ");
	LOG(println(SenderID,DEC));
	LOG_PRINT(" ->sensor type: ");
	LOG(println(Message->SensorType,DEC));
	LOG_PRINT(" ->API version: ");
	LOG(println(Message->ApiVersion,DEC));
	LOG_PRINT(" ->MeasurementPeriod: ");
	LOG(println(Message->MeasurementPeriod,DEC));
	LOG_PRINT(" ->isConfigured: ");
	LOG(print(Message->isConfigured,DEC));
	LOG_PRINT(" isRunning: ");
	LOG(print(Message->isRunning,DEC));
	LOG_PRINT(" isMeasurementValid: ");
	LOG(print(Message->isMeasurementValid,DEC));
	LOG_PRINT(" EventMask: ");
	LOG(println(Message->EventsMask,BIN));
	   //TODO: send a message
}

void tIncomingFrameHanlder::HandleMsgGetSensorMeasurementReqest(uint8_t SenderID, tMessageGetSensorMeasurementReqest *Message)
{
    /* get data blob from sensor and send it in one or more frames */
    tSensor *pSensor = tSensor::getSensor(Message->SensorID);
    if (NULL == pSensor)
        return;

    pSensor->sendMsgSensorEvent(true);
}

void tIncomingFrameHanlder::HandleMsgSensorEvent(uint8_t SenderID, tMessageSensorEvent *Message)
{
	DEBUG_PRINT_3("Sensor ID:");
	DEBUG_3(print(Message->Header.SensorID, DEC));
	DEBUG_PRINT_3(" EventType:");
	DEBUG_3(print(Message->Header.EventType, DEC));
	DEBUG_PRINT_3(" onDemand:");
	DEBUG_3(print(Message->Header.onDemand, DEC));
	DEBUG_PRINT_3(" SegmentSeq:");
	DEBUG_3(print(Message->Header.SegmentSeq, DEC));
	DEBUG_PRINT_3(" LastSegment:");
	DEBUG_3(println(Message->Header.LastSegment, DEC));
	DEBUG_PRINT_3(" Payload:");
	for (uint8_t i = 0; i < SENSOR_MEASUREMENT_PAYLOAD_SIZE; i++)
	{
		if (Message->Payload[i] < 0x10)
			DEBUG_PRINT_3("0");
		DEBUG_3(print(Message->Payload[i], HEX));
	}
	DEBUG_PRINTLN_3("");

    /* todo: put incoming data to SensorHub */
}


#endif //CONFIG_SENSORS

#endif // CONFIG_TLE8457_COMM_LIB
