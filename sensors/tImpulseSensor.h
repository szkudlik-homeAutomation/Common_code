/*
 * tImpulseSensor.h
 *
 *  Created on: 16 paü 2022
 *      Author: szkud
 */

#ifndef SRC_COMMON_CODE_SENSORS_TIMPULSESENSOR_H_
#define SRC_COMMON_CODE_SENSORS_TIMPULSESENSOR_H_

#include "tSensor.h"

class tImpulseSensor final : public tSensor {
public:
   typedef struct
   {
      uint8_t Pin;   // pin that is capable of HW interrupts
   } tImpulseSensorConfig;

   typedef struct
   {
      int16_t Count; // numer of impulses in last
   } tDS1820Result;

   tImpulseSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_IMPULSE,sensorID) {}
   virtual ~tImpulseSensor() {}
   virtual void SetSpecificConfig(void *pBlob);

protected:
   virtual void doTriggerMeasurement();
};

#endif /* SRC_COMMON_CODE_SENSORS_TIMPULSESENSOR_H_ */
