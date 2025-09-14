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
	   typedef struct
	   {
		   uint8_t NumOfPins;
	       uint8_t State[MAX_NUM_OF_PINS];
	       uint16_t Timer[MAX_NUM_OF_PINS];
	   } tResult_api_v1;

};
#endif

#if CONFIG_OUTPUT_STATE_SENSOR
class tOutputStateSensor : public tSensor, public tOutputStateSensorTypes {
public:
   tOutputStateSensor(uint8_t sensorID);

   static const uint8_t API_VERSION = 1;
   typedef tResult_api_v1 tResult;

protected:
   virtual void doTriggerMeasurement();
   virtual uint8_t onSetConfig();
private:
   tResult mResult;
};

#endif //CONFIG_OUTPUT_STATE_SENSOR
