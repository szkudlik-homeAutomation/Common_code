/*
 * tSensorStateServlet.h
 *
 *  Created on: 1 lis 2022
 *      Author: szkud
 */

#ifndef SRC_COMMON_CODE_SENSORS_TSENSORSTATESERVLET_H_
#define SRC_COMMON_CODE_SENSORS_TSENSORSTATESERVLET_H_

#include "../../../global.h"
#include "../../lib/TCP_Communication_lib/httpServer.h"


class tSensorStateServlet final : public tHttpServlet {
public:
   tSensorStateServlet() : tHttpServlet() {};
   virtual ~tSensorStateServlet() {};

   virtual bool ProcessAndResponse();

};

#endif /* SRC_COMMON_CODE_SENSORS_TSENSORSTATESERVLET_H_ */
