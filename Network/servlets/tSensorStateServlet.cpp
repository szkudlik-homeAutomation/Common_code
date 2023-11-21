/*
 * tSensorStateServlet.cpp
 *
 *  Created on: 1 lis 2022
 *      Author: szkud
 */

#include "../../../../global.h"
#if CONFIG_SENSOR_STATE_SERVLET

#if !CONFIG_SENSOR_HUB
#error CONFIG_SENSOR_STATE_SERVLET requires CONFIG_SENSOR_HUB
#endif

#include "tSensorStateServlet.h"
#include "../../sensors/tSensorHub.h"


bool tSensorStateServlet::ProcessAndResponse()
{
   pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"Sensors\":{\r\n"));

   tSensorHub::Instance->getCachedSensorsDataJson(&pOwner->mEthernetClient);

   pOwner->SendFlashString(PSTR("}}\r\n"));

   return false;
}

#endif //CONFIG_SENSOR_STATE_SERVLET
