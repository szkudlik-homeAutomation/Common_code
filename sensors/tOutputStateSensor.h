/*
 * tOutputStateSensor.h
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"
#if CONFIG_OUTPUT_STATE_SENSOR
#include "tSensor.h"
#include "tSensorCache.h"

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t OutputStateSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache);
#endif //CONFIG_SENSORS_JSON_OUTPUT

class tOutputStateSensor : public tSensor {
public:
   tOutputStateSensor(uint8_t sensorID);

   typedef struct
   {
      uint8_t State[CONFIG_OUTPUT_PROCESS_NUM_OF_PINS];
      uint16_t Timer[CONFIG_OUTPUT_PROCESS_NUM_OF_PINS];
   } tResult_api_v1;

   static const uint8_t API_VERSION = 1;
   typedef tResult_api_v1 tResult;

protected:
   virtual void doTriggerMeasurement();
   virtual uint8_t onSetConfig();
private:
   tResult mResult;
};

#endif //CONFIG_OUTPUT_STATE_SENSOR
