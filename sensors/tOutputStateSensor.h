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
    tOutputStateSensorDesc(uint8_t aSensorID, char * apSensorName) :
        tSensorDesc(SENSOR_TYPE_OUTPUT_STATES, aSensorID, apSensorName) {}

protected:
#if CONFIG_SENSORS_JSON_OUTPUT
   /* sensor specific JSON formatter */
    virtual uint8_t doFormatJSON(Stream *pStream);
#endif // CONFIG_SENSORS_JSON_OUTPUT
};

#endif CONFIG_SENSOR_HUB

class tOutputStateSensor : public tSensor {
public:
   static const uint8_t API_VERSION = 1;

   tOutputStateSensor();

   typedef struct
   {
      uint8_t State[NUM_OF_OUTPUTS];
      uint16_t Timer[NUM_OF_OUTPUTS];
   } tResult;

protected:
   virtual void doTriggerMeasurement();

private:
   tResult mResult;
};

#endif //CONFIG_OUTPUT_STATE_SENSOR
