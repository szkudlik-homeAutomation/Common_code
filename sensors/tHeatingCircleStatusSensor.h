/*
 * tHeatingCircleStatusSensor.h
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"
#include "tSensor.h"


class tHeatingCircleControl;
class tHeatingCircleStatusSensor : public tSensor {
public:
   tHeatingCircleStatusSensor() : tSensor(SENSOR_TYPE_OUTPUT_STATES) {}

   typedef struct
   {
      uint8_t isWorking;
      float TargetTemp;
   } tResult;

   typedef struct
   {
      tHeatingCircleControl *pHeatingControl;
   } tConfig;

   virtual void doTriggerMeasurement();
   virtual uint8_t SetSpecificConfig(void *pBlob);
#if CONFIG_SENSORS_JSON_OUTPUT
   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
#endif //CONFIG_SENSORS_JSON_OUTPUT

private:
   tResult mResult;
   tHeatingCircleControl *mHeatingControl;
};

