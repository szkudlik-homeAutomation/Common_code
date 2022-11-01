/*
 * tPt100AnalogSensor.h
 *
 *  Created on: 21 paü 2022
 *      Author: szkud
 */

#ifndef SRC_COMMON_CODE_SENSORS_TANALOGINPUTSENSOR_H_
#define SRC_COMMON_CODE_SENSORS_TANALOGINPUTSENSOR_H_

#include "../../../global.h"
#include "tSensor.h"

class tPt100AnalogSensor : public tSensor {
public:
   typedef struct
   {
      uint8_t Pin;
   } tConfig;

   typedef struct
   {
      int Temperature;
   } tResult;

   tPt100AnalogSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_PT100_ANALOG,sensorID) {}

   virtual void doTriggerMeasurement();
   virtual void SetSpecificConfig(void *pBlob);

   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);

private:
   tResult mResult;
   uint8_t mPin;
};

#endif /* SRC_COMMON_CODE_SENSORS_TANALOGINPUTSENSOR_H_ */
