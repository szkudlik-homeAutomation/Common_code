/*
 * tImpulseSensor.h
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#ifndef SRC_COMMON_CODE_SENSORS_TIMPULSESENSOR_H_
#define SRC_COMMON_CODE_SENSORS_TIMPULSESENSOR_H_

#include "../../../global.h"
#include "tSensor.h"

class tImpulseSensor final : public tSensor {
public:
   typedef struct
   {
      uint16_t Count; // numer of impulses in last period
      uint16_t Sum;   // sum of all impulses
   } tResult;

   tImpulseSensor(uint8_t sensorID);
   virtual ~tImpulseSensor() {}

   void CleanSum(); 			// clear Sum
   void Impulse() { mCnt++;	}	// COUNTER TRIGGER - interrupt safe, may be called in interrupt handler

   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);

protected:
   virtual void doTriggerMeasurement();
private:
   volatile uint16_t mCnt;
   tResult mResult;
};

#endif /* SRC_COMMON_CODE_SENSORS_TIMPULSESENSOR_H_ */
