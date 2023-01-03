/*
 * tSensorStateServlet.h
 *
 *  Created on: 1 lis 2022
 *      Author: szkud
 */

#pragma once


#include "../../../../global.h"

#if CONFIG_SENSOR_STATE_SERVLET
#include "../httpServer.h"


class tSensorStateServlet final : public tHttpServlet {
public:
   tSensorStateServlet() : tHttpServlet() {};
   virtual ~tSensorStateServlet() {};

   virtual bool ProcessAndResponse();

};

#endif //CONFIG_SENSOR_STATE_SERVLET
