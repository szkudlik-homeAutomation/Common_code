/*
 * tImpulseSensor.h
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#ifndef SRC_COMMON_CODE_SENSORS_TIMPULSESENSOR_H_
#define SRC_COMMON_CODE_SENSORS_TIMPULSESENSOR_H_

#include "tSensor.h"

class tImpulseSensor final : public tSensor {
public:
   typedef struct
   {
      uint8_t Pin;   // pin that is capable of PCINT
      bool    ContinousCnt;	// if true the counter won't be cleaned after every period (sum of all impulses since start)
   } tImpulseSensorConfig;

   typedef struct
   {
      int16_t Count; // numer of impulses in last/sum
   } tResult;

   tImpulseSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_IMPULSE,sensorID) {}
   virtual ~tImpulseSensor() {}
   virtual void SetSpecificConfig(void *pBlob);

   void CleanCnt() {}		// makes sense in case of ContinousCnt - clean a counter

protected:
   virtual void doTriggerMeasurement();
private:
   volatile uint16_t mCnt;
   uint16_t mShadowCnt;
};

#endif /* SRC_COMMON_CODE_SENSORS_TIMPULSESENSOR_H_ */
