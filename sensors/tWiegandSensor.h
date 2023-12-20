/*
 * tWiegandSensor.h
 *
 *  Created on: 21 pa� 2022
 *      Author: szkud
 */

#pragma once


#include "../../../global.h"
#if CONFIG_WIEGAND_SENSOR
#include "tSensor.h"
#include "tSensorDesc.h"
#include "../../lib/Wiegand/src/Wiegand.h"

#if CONFIG_SENSOR_HUB
class tWiegandSensorDesc : public tSensorDesc
{
public:
    tWiegandSensorDesc() : tSensorDesc() {}

protected:
#if CONFIG_SENSORS_JSON_OUTPUT
   /* sensor specific JSON formatter */
    virtual uint8_t doFormatJSON(Stream *pStream);
#endif // CONFIG_SENSORS_JSON_OUTPUT
};
#endif //CONFIG_SENSOR_HUB


class tWiegandSensor : public tSensor {
public:
   typedef struct
   {
      uint8_t PinD0;
      uint8_t PinD1;
   } tConfig_api_v1;

   typedef struct
   {
	   uint32_t code;
	   uint8_t type;
   } tResult_api_v1;

   static const uint8_t API_VERSION = 1;
   typedef tConfig_api_v1 tConfig;
   typedef tResult_api_v1 tResult;

   tConfig Config;

   tWiegandSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_WIEGAND, sensorID, API_VERSION, sizeof(Config), &Config)
   {
	   mCurrentMeasurementBlob = (void*) &mResult;
	   mMeasurementBlobSize = sizeof(mResult);
   }

protected:
   virtual void doTriggerMeasurement();
   virtual uint8_t onSetConfig();
private:
   WIEGAND Wiegant;
   tResult mResult;
};

#endif // CONFIG_WIEGAND_SENSOR
