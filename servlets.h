/*
 * servlets.h
 *
 *  Created on: 23 wrz 2022
 *      Author: szkud
 */

#ifndef SRC_SERVLETS_H_
#define SRC_SERVLETS_H_

#include "../../global.h"
#include "../lib/TCP_Communication_lib/httpServer.h"
#include "sensors/tDS1820Sensor.h"

/**
 * output servlet - controlling or checking state of an output
 */
class tDS1820Servlet :  public tHttpServlet
{
public:
   static const uint8_t MAX_NUM_OF_SENSORS = 8;

   tDS1820Servlet() : tHttpServlet() {}
   virtual ~tDS1820Servlet() {}

   static void registerSensor(tDS1820Sensor *pSensor);

   virtual bool ProcessAndResponse();
private:
   static tDS1820Sensor *mSensors[MAX_NUM_OF_SENSORS];
   static uint8_t mNumOfSensors;
};


#endif /* SRC_SERVLETS_H_ */
