/*
 * tOutputStateSensor.h
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"
#include "tSensor.h"
#include "tSensorCache.h"


#if CONFIG_OUTPUT_STATE_SENSOR_JSON_OUTPUT
class tSensorJsonFormatter_OutputState_api_1 : public tSensorJsonFormatter
{
public:
	tSensorJsonFormatter_OutputState_api_1() : tSensorJsonFormatter() {}
protected:
	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) override;
	virtual const char *getSensorTypeName() override;
};
#endif //CONFIG_OUTPUT_STATE_SENSOR_JSON_OUTPUT

#if CONFIG_OUTPUT_STATE_SENSOR || CONFIG_OUTPUT_STATE_SENSOR_JSON_OUTPUT
class tOutputStateSensorTypes
{
public:

	   static const uint8_t MAX_NUM_OF_PINS = 16;
	   template<int SIZE>
	   struct tResult_api_v1
	   {
		   uint8_t NumOfPins;
	       uint16_t StateBitmap;
	       uint16_t Timer[SIZE];
	   };

	   static uint8_t getResultSize(uint8_t NumOfPins)
	   {
			if (NumOfPins > MAX_NUM_OF_PINS)
			   return 0;
		    return sizeof(tResult_api_v1<0>) + sizeof(uint16_t) * NumOfPins;
	   }
};
#endif

#if CONFIG_OUTPUT_STATE_SENSOR
class tOutputStateSensor : public tSensor, public tOutputStateSensorTypes {
public:
   tOutputStateSensor(uint8_t sensorID);

   static const uint8_t API_VERSION = 1;
   using tResult = tResult_api_v1<MAX_NUM_OF_PINS>;

protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();
   virtual uint8_t onSetConfig();
private:
   tResult mResult;
};

#endif //CONFIG_OUTPUT_STATE_SENSOR
