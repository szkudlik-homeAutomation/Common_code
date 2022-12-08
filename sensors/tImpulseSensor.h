/*
 * tImpulseSensor.h
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"
#include "tSensor.h"

class tImpulseSensor final : public tSensor {
public:
   typedef struct
   {
      uint16_t Count; // numer of impulses in last period
      uint16_t Sum;   // sum of all impulses
   } tResult;

   tImpulseSensor();
   virtual ~tImpulseSensor() {}

   void CleanSum(); 			// clear Sum
   void Impulse() { mCnt++;	}	// COUNTER TRIGGER - interrupt safe, may be called in interrupt handler

#if CONFIG_SENSORS_JSON_OUTPUT
   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
#endif //CONFIG_SENSORS_JSON_OUTPUT

protected:
   virtual void doTriggerMeasurement();
private:
   volatile uint16_t mCnt;
   tResult mResult;
};
