/*
 * tOutputSetServlet.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: mszkudli
 */
#include "../../../../global.h"

#if CONFIG_OUTPUT_CONTROL_SERVLET

#include "tOutputControlServlets.h"
#include "../../TLE8457_serial/tOutgoingFrames.h"

#if CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL
#include "../../TLE8457_serial/TLE8457_serial_lib.h"
#endif // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL

bool tOutputSetServlet::ProcessAndResponse()
{
   uint16_t Output_or_OutputMask;  // output mask, bitmap
   bool isMask = false;
   
   uint16_t State_or_StateBitmap;  // output mask, bitmap
   uint16_t Timer = 0; // TODO DEFAULT_TIMER;
   uint16_t Device;
   bool ParametersOK = true;
   bool ExternalDevice = false;

   // there may be single output or multiple outputs specified
   // url for single output: ...?Out=<output_id>&State=<state>&Timer=<timer_value> or Dev=<device_id>&Out=<output_id>&State=<state>&Timer=<timer_value>

   // url for bitmap:
   // ...?OutMask=<output_mask>&State=<state_bitmap>&Timer=<timer_value common for all outputs> or Dev=<device_id>&OutMask=<output_mask>&State=<state_bitmap>&Timer=<timer_value common for all outputs>

   #if CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL
      ExternalDevice = GetParameter("Dev",&Device); // check if external device parameter is provided
   #endif // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL

   if (GetParameter("OutMask",&Output_or_OutputMask))
   {
       isMask = true;
       ParametersOK &= GetParameter("State",&State_or_StateBitmap);
   }
   else
   {
       ParametersOK &= GetParameter("Out",&Output_or_OutputMask);
       ParametersOK &= GetParameter("State",&State_or_StateBitmap);
   }
   GetParameter("Timer",&Timer);    // optional

   if (! ParametersOK)
   {
     SendResponse400();
     return false;
   }

   #if DEBUG_LOGLEVEL_2   
      if (isMask)
      {
         DEBUG_PRINT_2("==>HTTP set output, OutputMask=");
         DEBUG_2(print(Output_or_OutputMask,BIN));
         DEBUG_PRINT_2(" StateMask=");
         DEBUG_2(print(State_or_StateBitmap,BIN));
         DEBUG_PRINT_2(" Timer=");
         DEBUG_2(println(Timer,DEC));
      }
      else
      {
         DEBUG_PRINT_2("==>HTTP set output, Output=");
         DEBUG_2(print(Output_or_OutputMask,DEC));
         DEBUG_PRINT_2(" State=");
         DEBUG_2(print(State_or_StateBitmap,DEC));
         DEBUG_PRINT_2(" Timer=");
         DEBUG_2(println(Timer,DEC));
      }
   #endif // DEBUG_LOGLEVEL_2

   if (ExternalDevice)
   {
      // send command to external device
      #if CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL
      if (isMask)
         tOutgoingFrames::SendMsgSetOutputMask(Device, Output_or_OutputMask, State_or_StateBitmap, Timer);
      else         
         tOutgoingFrames::SendMsgSetOutput(Device, Output_or_OutputMask, State_or_StateBitmap, Timer);

         SendResponse200();
      #else
         // handle case when TLE8457 serial is not used
         SendResponse501();
      #endif // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL      

      return false;
   }

   #if CONFIG_OUTPUT_PROCESS
      // local device handling, check if output process instance is available
      if (NULL == tOutputProcess::Instance && !ExternalDevice) 
      {
         SendResponse501();
         return false;
      }

      if (isMask)
      {
         for(uint8_t out = 0; out < 16; out++)
         {
            if ((Output_or_OutputMask & (1 << out)) == 0)
               continue;

            uint8_t state = (State_or_StateBitmap & ((uint16_t)1 << out)) ? 1 : 0;

            tOutputProcess::Instance->SetOutput(out, state, Timer, tOutputProcess::ForceTimer);
         }
      }
      else
      {
         tOutputProcess::Instance->SetOutput(Output_or_OutputMask,(State_or_StateBitmap ? 1 : 0),Timer,tOutputProcess::ForceTimer);
      }
      SendResponse200();
   #else // CONFIG_OUTPUT_PROCESS
      SendResponse501();
   #endif // CONFIG_OUTPUT_PROCESS

   return false;
}

bool tOutputStateServlet::ProcessAndResponse()
{
   uint16_t Output;  // output iD
   uint8_t State;
   uint16_t TimerValue;
   bool ParametersOK = true;

   ParametersOK &= GetParameter("Out",&Output);

   if (! ParametersOK)
   {
     SendResponse400();
     return false;
   }


   if (NULL == tOutputProcess::Instance)
   {
     SendResponse501();
     return false;
   }

   ParametersOK &= (Output < tOutputProcess::Instance->getNumOfOutputs());

   if (! ParametersOK)
   {
     SendResponse400();
     return false;
   }

   State = tOutputProcess::Instance->GetOutputState(Output);
   TimerValue = tOutputProcess::Instance->GetOutputTimer(Output);


   DEBUG_PRINT_2("==>HTTP output state get, Out=");
   DEBUG_2(print(Output,DEC));
   DEBUG_PRINT_2(" State=");
   DEBUG_2(print(State,DEC));
   DEBUG_PRINT_2(" Timer=");
   DEBUG_2(print(TimerValue,DEC));

   sendJsonResponse(0, Output, State, TimerValue, 0);

   return false;
}


void tOutputStateServlet::sendJsonResponse(uint16_t devID, uint16_t Output, uint16_t State, uint16_t TimerValue, uint16_t DefaultTimer)
{
	   // format JSON response
	   pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"Device\": "));
	   pOwner->mEthernetClient.print(devID,DEC);
	   pOwner->SendFlashString(PSTR(", \"Output\": "));
	   pOwner->mEthernetClient.print(Output,DEC);
	   pOwner->SendFlashString(PSTR(", \"State\": "));
	   pOwner->mEthernetClient.print(State,DEC);
	   pOwner->SendFlashString(PSTR(", \"Timer\": "));
	   pOwner->mEthernetClient.print(TimerValue,DEC);
	   pOwner->SendFlashString(PSTR(", \"DefaultTimer\": "));
	   pOwner->mEthernetClient.print(DefaultTimer,DEC);
	   pOwner->SendFlashString(PSTR("}\r\n"));

}

#if CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL

void tOutputStateTLE8457Servlet::onMessage(uint8_t type, uint16_t data, void *pData)
{
	if (type != MessageType_SerialFrameRecieved || data != MESSAGE_TYPE_OUTPUT_STATE_RESPONSE)
		return;

	tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    tMessageTypeOutputStateResponse *OutputStateResponse = (tMessageTypeOutputStateResponse *)pFrame->Data;

    if (pFrame->SenderDevId != mExpectedDevID) return;
    if (OutputStateResponse->OutputID != mExpectedOutputID) return;
    mPowerState = OutputStateResponse->PowerState;
    mTimerValue = OutputStateResponse->TimerValue;
    mDefaultTimer = OutputStateResponse->DefaultTimer;
}


void tOutputStateTLE8457Servlet::SendOutputStateRequest(uint8_t DevID, uint8_t OutputID)
{
  mPowerState = 255;
  mStartTimestamp = millis();
  mExpectedDevID = DevID;
  mExpectedOutputID = OutputID;

  tOutgoingFrames::SendMsgOutputStateRequest(DevID,OutputID);
}

uint8_t tOutputStateTLE8457Servlet::CheckStateRequest()
{
  if ((millis() - mStartTimestamp) > CONFIG_OUTPUT_CONTROL_SERVLET_TLE8457_SERIAL_TIMEOUT)
	  return STATUS_TIMEOUT;
  if (mPowerState == 255) return STATUS_WAIT;
  return STATUS_COMPLETE;
}

bool tOutputStateTLE8457Servlet::ProcessAndResponse()
{
   uint16_t Device;  // device iD
   uint16_t Output;  // output iD
   if (false == mRequestSent)
   {
	  if (!GetParameter("Dev",&Device))
		  // pass to base class
		  return tOutputStateServlet::ProcessAndResponse();

      bool ParametersOK = true;
      ParametersOK &= GetParameter("Out",&Output);

      if (! ParametersOK)
      {
        SendResponse400();
        return false;
      }

      DEBUG_PRINT_2("==>HTTP output state get, Dev=");
      DEBUG_2(print(Device,DEC));
      DEBUG_PRINT_2(" Out=");
      DEBUG_2(println(Output,DEC));
      SendOutputStateRequest(Device,Output);
      mRequestSent = true;
      return true;   // wait for result
   }

   uint8_t Status = CheckStateRequest();
   switch (Status)
   {
      case STATUS_WAIT:
         return true; // keep waiting

      case STATUS_COMPLETE:
         break;

      case STATUS_TIMEOUT:
      default:
    	  SendResponse424();
          return false;
   }

   // format JSON response
   sendJsonResponse(mExpectedDevID, mExpectedOutputID, mPowerState, mTimerValue, mDefaultTimer);

   return false;
}

#endif // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL

#endif // CONFIG_OUTPUT_CONTROL_SERVLET
