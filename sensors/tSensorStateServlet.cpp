/*
 * tSensorStateServlet.cpp
 *
 *  Created on: 1 lis 2022
 *      Author: szkud
 */

#include "../../../global.h"
#if CONFIG_SENSOR_STATE_SERVLET

#include "tSensorStateServlet.h"
#include "tSensorHub.h"


bool tSensorStateServlet::ProcessAndResponse()
{
   pOwner->SendFlashString(PSTR("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"Sensors\":{\r\n"));

   SensorHub.getCachedSensorsDataJson(&pOwner->mEthernetClient);

   pOwner->SendFlashString(PSTR("}}\r\n"));

   return false;
}

#endif //CONFIG_SENSOR_STATE_SERVLET
