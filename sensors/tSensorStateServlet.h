/*
 * tSensorStateServlet.h
 *
 *  Created on: 1 lis 2022
 *      Author: szkud
 */

#pragma once


#include "../../../global.h"
#include "../Network/httpServer.h"


class tSensorStateServlet final : public tHttpServlet {
public:
   tSensorStateServlet() : tHttpServlet() {};
   virtual ~tSensorStateServlet() {};

   virtual bool ProcessAndResponse();

};
