/*
 * tImpulseSensor.h
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"
#if CONFIG_IMPULSE_SENSOR

#include "tSensor.h"
#include "tSensorDesc.h"

class tImpulseSensorDesc : public tSensorDesc
{
public:
    tImpulseSensorDesc() : tSensorDesc() {}

protected:
#if CONFIG_SENSORS_JSON_OUTPUT
   /* sensor specific JSON formatter */
    virtual uint8_t doFormatJSON(Stream *pStream);
#endif // CONFIG_SENSORS_JSON_OUTPUT
};

class tImpulseSensor final : public tSensor {
public:
   typedef struct
   {
      uint16_t Count; // numer of impulses in last period
      uint16_t Sum;   // sum of all impulses
   } tResult_api_v1;

   static const uint8_t API_VERSION = 1;
   typedef tResult_api_v1 tResult;


   tImpulseSensor(uint8_t sensorID);
   virtual ~tImpulseSensor() {}

   void CleanSum(); 			// clear Sum
   void Impulse() { mCnt++;	}	// COUNTER TRIGGER - interrupt safe, may be called in interrupt handler

protected:
   virtual void doTriggerMeasurement();
private:
   volatile uint16_t mCnt;
   tResult mResult;
};

#endif //CONFIG_IMPULSE_SENSOR
