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

bool tOutputSetServlet::ProcessAndResponse()
{
   uint8_t Status;
   uint16_t Output;  // output iD
   uint16_t State;  // 1 - on, 0 - off
   uint16_t Timer = 0; // TODO DEFAULT_TIMER;
   uint16_t Device;
   bool ParametersOK = true;

#if CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL
   bool ExternalDevice = false;
   ExternalDevice = GetParameter("Dev",&Device);
#endif // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL

   ParametersOK &= GetParameter("Out",&Output);
   ParametersOK &= GetParameter("State",&State);
   GetParameter("Timer",&Timer);    // optional

   if (! ParametersOK)
   {
     SendResponse400();
     return false;
   }

   DEBUG_PRINT_2("==>HTTP set output, Out=");
   DEBUG_2(print(Output,DEC));
   DEBUG_PRINT_2(" State=");
   DEBUG_2(print(State,DEC));
   DEBUG_PRINT_2(" Timer=");
   DEBUG_2(println(Timer,DEC));

#if CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL
   if (ExternalDevice)
   {
	   tOutgoingFrames::SendMsgSetOutput(Device, Output, State, Timer);
	   SendResponse200();
	   return false;
   }
#endif // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL

#if CONFIG_OUTPUT_PROCESS
   if (NULL != tOutputProcess::Instance) {
	   Status = tOutputProcess::Instance->SetOutput(Output,State,Timer,tOutputProcess::ForceTimer);
		if (STATUS_SUCCESS == Status) {
			SendResponse200();
		} else {
			SendResponse400();
		}
	   return false;
   }
#endif // CONFIG_OUTPUT_PROCESS

   SendResponse501();
   return false;
}


bool tOutputStateServlet::ProcessAndResponse()
{
   uint16_t Output;  // output iD
   uint8_t State;
   uint16_t TimerValue;
   uint16_t DefaultTimer = 1000;
   bool ParametersOK = true;

#if CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL
   bool ExternalDevice = false;
   ExternalDevice = GetParameter("Dev",&Device);
#endif // CONFIG_OUTPUT_CONTROL_SERVLET_USE_TLE8457_SERIAL
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

   ParametersOK &= (Output < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS);

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
   DEBUG_PRINT_2(" DefaultTimer=");
   DEBUG_2(println(DefaultTimer,DEC));

   //   // format JSON response
   pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"Output\": "));
   pOwner->mEthernetClient.print(Output,DEC);
   pOwner->SendFlashString(PSTR(", \"State\": "));
   pOwner->mEthernetClient.print(State,DEC);
   pOwner->SendFlashString(PSTR(", \"Timer\": "));
   pOwner->mEthernetClient.print(TimerValue,DEC);
   pOwner->SendFlashString(PSTR(", \"DefaultTimer\": "));
   pOwner->mEthernetClient.print(DefaultTimer,DEC);
   pOwner->SendFlashString(PSTR("}\r\n"));

   return false;
}

#endif // CONFIG_OUTPUT_CONTROL_SERVLET
