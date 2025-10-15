/*
 * sensorJsonOutput.cpp
 *
 *  Created on: 17 cze 2025
 *      Author: szkud
 */

#include "../../../global.h"
#include "tSensorJsonOutput.h"
#include "tSensor.h"

#include "tDS1820Sensor.h"
#include "tImpulseSensor.h"
#include "tPt100AnalogSensor.h"
#include "tSimpleDigitalInputSensor.h"
#include "tOutputStateSensor.h"
#include "tSystemStatusSensor.h"
#include "tWiegandSensor.h"
#include "tSht3Sensor.h"
#include "tTgs2603AnalogSensor.h"

#if CONFIG_SENSORS_JSON_OUTPUT

tSensorJsonFormatterFactory* tSensorJsonFormatterFactory::Instance = NULL;


const char *tSensorJsonFormatter::getSensorTypeName()
{
	static const char IdPrefix[] PROGMEM = "Sensor";
	return IdPrefix;
}

tSensorJsonFormatter *tSensorJsonFormatterFactory::createJsonFormatter(uint8_t SensorType, uint8_t apiVersion)
{
	switch (SensorType)
	{
	#if CONFIG_DS1820_SENSOR_JSON_OUTPUT
		  case SENSOR_TYPE_DS1820:
			  switch (apiVersion)
			  {
			  case 1:
				  return new tSensorJsonFormatter_DS1820_api_1;
			  }
			  break;
	#endif // CONFIG_DS1820_SENSOR

	#if CONFIG_IMPULSE_SENSOR_JSON_OUTPUT
		  case SENSOR_TYPE_IMPULSE:
			  switch (apiVersion)
			  {
			  case 1:
				  return new tSensorJsonFormatter_ImpulseSensor_api_1;
			  }
			  break;
	#endif //CONFIG_IMPULSE_SENSOR

	#if CONFIG_PT100_ANALOG_SENSOR_JSON_OUTPUT
	      case SENSOR_TYPE_PT100_ANALOG:
	          switch (apiVersion)
	          {
	          case 1:
	              return new tSensorJsonFormatter_AnalogSensor_api_1;
	          }
	          break;
	#endif //CONFIG_PT100_ANALOG_SENSOR

	#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR_JSON_OUTPUT
	      case SENSOR_TYPE_DIGITAL_INPUT:
	          switch (apiVersion)
	          {
	          case 1:
	              return new tSensorJsonFormatter_SimpleDigitalInput_api_1;
	          }
	          break;
	#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR

	#if CONFIG_OUTPUT_STATE_SENSOR_JSON_OUTPUT
	      case SENSOR_TYPE_OUTPUT_STATES:
	          switch (apiVersion)
	          {
	          case 1:
	              return new tSensorJsonFormatter_OutputState_api_1;
	          }
	          break;
	#endif //CONFIG_OUTPUT_STATE_SENSOR

	#if CONFIG_SYSTEM_STATUS_SENSOR_JSON_OUTPUT
	      case SENSOR_TYPE_SYSTEM_STATUS:
	          switch (apiVersion)
	          {
	          case 1:
	              return new tSensorJsonFormatter_SystemStatus_api_1;
	          case 2:
	              return new tSensorJsonFormatter_SystemStatus_api_2;
	          }
	          break;
	#endif //CONFIG_SYSTEM_STATUS_SENSOR

	#if CONFIG_SHT3_DIS_SENSOR_JSON_OUTPUT
	      case SENSOR_TYPE_SHT3:
	    	  switch (apiVersion)
	    	  {
	    	  case 1:
	    		  return new tSensorJsonFormatter_Sht3Sensor_api_1;
	    	  }
	    	  break;
	#endif //CONFIG_SHT3_DIS_SENSOR_JSON_OUTPUT

	#if CONFIG_TGS2603_ODOUR_SENSOR_JSON_OUTPUT
	   case SENSOR_TYPE_TGS2603:
		   	 switch (apiVersion)
		   	 {
		   	 case 1:
		   		 return new tSensorJsonFormatter_Tgs2603OdourSensor_api_1;
		   	 }
		   	 break;
	#endif //CONFIG_TGS2603_ODOUR_SENSOR_JSON_OUTPUT

	  default:
		  return appSpecificCreateJsonFormatter(SensorType, apiVersion);
	}

	return NULL;
}

#if CONFIG_SENSORS_JSON_OUTPUT_INSTANCE
tSensorJsonFormatterFactory SensorJsonFormatterFactory;
#endif //CONFIG_SENSORS_JSON_OUTPUT_INSTANCE

#endif // CONFIG_SENSORS_JSON_OUTPUT
