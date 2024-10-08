/*
 * tOutputSetServlet.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: mszkudli
 */

#include "../../../../global.h"

#if CONFIG_OUTPUT_CONTROL_SERVLET

#include "tOutputControlServlets.h"

bool tOutputSetServlet::ProcessAndResponse()
{
   uint16_t Output;  // output iD
   uint16_t State;  // 1 - on, 0 - off
   uint16_t Timer = 0; // TODO DEFAULT_TIMER;
   bool ParametersOK = true;

   ParametersOK &= GetParameter("Out",&Output);
   ParametersOK &= GetParameter("State",&State);
   GetParameter("Timer",&Timer);    // optional
   ParametersOK &= (Output < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS);

   if (! ParametersOK)
   {
     SendResponse400();
     return false;
   }

   DEBUG_PRINT_3("==>HTTP set output, Out=");
   DEBUG_3(print(Output,DEC));
   DEBUG_PRINT_3(" State=");
   DEBUG_3(print(State,DEC));
   DEBUG_PRINT_3(" Timer=");
   DEBUG_3(println(Timer,DEC));

   if (NULL != tOutputProcess::Instance) {
	   tOutputProcess::Instance->SetOutput(Output,State,Timer,tOutputProcess::ForceTimer);
	   SendResponse200();
   }
   else
   {
	   SendResponse501();
   }

return false;
}


bool tOutputStateServlet::ProcessAndResponse()
{
   uint16_t Output;  // output iD
   uint8_t State;
   uint16_t TimerValue;
   uint16_t DefaultTimer = 1000;
   bool ParametersOK = true;

   if (NULL == tOutputProcess::Instance)
   {
     SendResponse501();
     return false;
   }

   ParametersOK &= GetParameter("Out",&Output);
   ParametersOK &= (Output < CONFIG_OUTPUT_PROCESS_NUM_OF_PINS);

   if (! ParametersOK)
   {
     SendResponse400();
     return false;
   }

   State = tOutputProcess::Instance->GetOutputState(Output);
   TimerValue = tOutputProcess::Instance->GetOutputTimer(Output);


   DEBUG_PRINT_3("==>HTTP output state get, Out=");
   DEBUG_3(print(Output,DEC));
   DEBUG_PRINT_3(" State=");
   DEBUG_3(print(State,DEC));
   DEBUG_PRINT_3(" Timer=");
   DEBUG_3(print(TimerValue,DEC));
   DEBUG_PRINT_3(" DefaultTimer=");
   DEBUG_3(println(DefaultTimer,DEC));

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
