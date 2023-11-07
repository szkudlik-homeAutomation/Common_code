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
#include "tSensorDesc.h"

#if CONFIG_SENSOR_HUB
class tOutputStateSensorDesc : public tSensorDesc
{
public:
    tOutputStateSensorDesc() : tSensorDesc() {}

protected:
#if CONFIG_SENSORS_JSON_OUTPUT
   /* sensor specific JSON formatter */
    virtual uint8_t doFormatJSON(Stream *pStream);
#endif // CONFIG_SENSORS_JSON_OUTPUT
};

#endif CONFIG_SENSOR_HUB

class tOutputStateSensor : public tSensor {
public:
   tOutputStateSensor();

   typedef struct
   {
      uint8_t State[NUM_OF_OUTPUTS];
      uint16_t Timer[NUM_OF_OUTPUTS];
   } tResult_api_v1;

   static const uint8_t API_VERSION = 1;
   typedef tResult_api_v1 tResult;

protected:
   virtual void doTriggerMeasurement();

private:
   tResult mResult;
};

#endif //CONFIG_OUTPUT_STATE_SENSOR
