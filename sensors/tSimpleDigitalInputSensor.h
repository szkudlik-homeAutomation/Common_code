/*
 * tSimpleDigitalInputSensor.h
 *
 *  Created on: Nov 25, 2022
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"
#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorJsonOutput.h"

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR_JSON_OUTPUT
class tSensorJsonFormatter_SimpleDigitalInput_api_1 : public tSensorJsonFormatter
{
public:
	tSensorJsonFormatter_SimpleDigitalInput_api_1() : tSensorJsonFormatter() {}
protected:
	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) override;
	virtual const char *getSensorTypeName() override;
};

class tSensorJsonFormatter_SimpleDigitalInput_api_2 : public tSensorJsonFormatter_SimpleDigitalInput_api_1
{
public:
	tSensorJsonFormatter_SimpleDigitalInput_api_2() : tSensorJsonFormatter_SimpleDigitalInput_api_1() {}
protected:
	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) override;
};

#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR_JSON_OUTPUT

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR || CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR_JSON_OUTPUT
class tSimpleDigitalInputSensorTypes
{
public:
	   typedef struct
	   {
	      uint8_t Pin;
	      uint8_t ActiveState;
	   } tConfig_api_v1;

	   typedef struct
	   {
	      uint8_t Pin[CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR_NUM_OF_INPUTS];
	      uint16_t ActiveStateBitmap;
		  uint8_t NumOfInputs;
	   } tConfig_api_v2;

	   typedef struct
	   {
	      uint8_t State;
	   } tResult_api_v1;

	   typedef struct
	   {
	      uint16_t State;
		  uint8_t NumOfInputs;
	   } tResult_api_v2;

};
#endif


#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR

class tSimpleDigitalInputSensor: public tSensor, public tSimpleDigitalInputSensorTypes {
public:
   static const uint8_t API_VERSION = 2;
   typedef tConfig_api_v2 tConfig;
   typedef tResult_api_v2 tResult;

   tConfig Config;

   tSimpleDigitalInputSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_DIGITAL_INPUT, sensorID, API_VERSION, sizeof(Config), &Config) {}

protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();
   virtual uint8_t onSetConfig();
private:
   tResult mResult;
};

#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
