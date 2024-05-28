/*
 * tSensorFactory.cpp
 *
 *  Created on: 11 lis 2023
 *      Author: szkud
 */


#include "../../../global.h"
#if CONFIG_SENSORS

#include "tSensorFactory.h"
#include "tSensor.h"
#include "tSensorHub.h"

#if CONFIG_DS1820_SENSOR
#include "tDS1820Sensor.h"
#endif

#if CONFIG_IMPULSE_SENSOR
#include "tImpulseSensor.h"
#endif

#if CONFIG_PT100_ANALOG_SENSOR
#include "tPt100AnalogSensor.h"
#endif

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
#include "tSimpleDigitalInputSensor.h"
#endif

#if CONFIG_OUTPUT_STATE_SENSOR
#include "tOutputStateSensor.h"
#endif

#if CONFIG_SYSTEM_STATUS_SENSOR
#include "tSystemStatusSensor.h"
#endif

#if CONFIG_WIEGAND_SENSOR
#include "tWiegandSensor.h"
#endif

static tSensorFactory* tSensorFactory::Instance;

#if CONFIG_SENSORS_JSON_OUTPUT

doFormatJSON tSensorFactory::getJSONFormatFunction(uint8_t SensorType, uint8_t apiVersion)
{
    switch (SensorType)
    {
    #if CONFIG_DS1820_SENSOR
          case SENSOR_TYPE_DS1820:
              switch (apiVersion)
              {
              case 1:
                  return DS1820SensorJsonFormat_api_1;
              }
              break;
    #endif // CONFIG_DS1820_SENSOR

    #if CONFIG_IMPULSE_SENSOR
          case SENSOR_TYPE_IMPULSE:
              switch (apiVersion)
              {
              case 1:
                  return ImpulseSensorJsonFormat_api_1;
              }
              break;
    #endif //CONFIG_IMPULSE_SENSOR

#if CONFIG_PT100_ANALOG_SENSOR
      case SENSOR_TYPE_PT100_ANALOG:
          switch (apiVersion)
          {
          case 1:
              return Pt100AnalogSensorJsonFormat_api_1;
          }
          break;
#endif //CONFIG_PT100_ANALOG_SENSOR

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
      case SENSOR_TYPE_DIGITAL_INPUT:
          switch (apiVersion)
          {
          case 1:
              return SimpleDigitalInputSensorJsonFormat_api_1;
          }
          break;
#endif //CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR

#if CONFIG_OUTPUT_STATE_SENSOR
      case SENSOR_TYPE_OUTPUT_STATES:
          switch (apiVersion)
          {
          case 1:
              return OutputStateSensorJsonFormat_api_1;
          }
          break;
#endif //CONFIG_OUTPUT_STATE_SENSOR

#if CONFIG_SYSTEM_STATUS_SENSOR
      case SENSOR_TYPE_SYSTEM_STATUS:
          switch (apiVersion)
          {
          case 1:
              return SystemStatusSensorJsonFormat_api_1;
          }
          break;
#endif //CONFIG_SYSTEM_STATUS_SENSOR
          default:
              return appSpecificGetSJONFrormatFunction(SensorType, apiVersion);
    }

    return NULL;
}
#endif // CONFIG_SENSORS_JSON_OUTPUT

tSensor *tSensorFactory::CreateSensor(uint8_t SensorType, uint8_t SensorID, char *pName, uint8_t ApiVersion, void *pConfigBlob,
	      uint8_t configBlobSize, uint16_t measurementPeriod, bool autoStart)
{
	uint8_t Status;
	tSensor *pSensor = CreateSensor(SensorType, SensorID, pName);
	if (NULL == pSensor)
	{
		DEBUG_PRINTLN_3(" error: cannot create sensor");
		return NULL;
	}

	Status = pSensor->setConfig(measurementPeriod, ApiVersion, pConfigBlob, configBlobSize);
	if (STATUS_SUCCESS != Status)
	{
		// sensor cannot be delted, this situation is more/less fatal
		return NULL;
	}

	if(autoStart)
		pSensor->Start();

#if CONFIG_SENSOR_HUB
#if REMOTE_SENSORS_TEST
    if (SensorID == 1)
#endif // REMOTE_SENSORS_TEST
    	tSensorHub::Instance->RegisterSensor(SensorID);
#endif // CONFIG_SENSOR_HUB
	return pSensor;
}

tSensor *tSensorFactory::CreateSensor(uint8_t SensorType, uint8_t SensorID, char *pName)
{
	DEBUG_PRINT_3("Creating sensor type ");
	DEBUG_3(print(SensorType,DEC));
	DEBUG_PRINT_3(" ID ");
	DEBUG_3(print(SensorID,DEC));

	tSensor *pSensor = NULL;
    switch (SensorType)
    {
    #if CONFIG_DS1820_SENSOR
          case SENSOR_TYPE_DS1820:
        	  pSensor = new tDS1820Sensor(SensorID);
              break;
    #endif // CONFIG_DS1820_SENSOR

    #if CONFIG_IMPULSE_SENSOR
          case SENSOR_TYPE_IMPULSE:
        	 pSensor = new tImpulseSensor(SensorID);
             break;
    #endif //CONFIG_IMPULSE_SENSOR

    #if CONFIG_PT100_ANALOG_SENSOR
          case SENSOR_TYPE_PT100_ANALOG:
        	  pSensor = new tPt100AnalogSensor(SensorID);
              break;
    #endif

    #if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
          case SENSOR_TYPE_DIGITAL_INPUT:
        	  pSensor = new tSimpleDigitalInputSensor(SensorID);
              break;
    #endif

    #if CONFIG_OUTPUT_STATE_SENSOR
         case SENSOR_TYPE_OUTPUT_STATES:
        	 pSensor = new tOutputStateSensor(SensorID);
             break;
    #endif

    #if CONFIG_SYSTEM_STATUS_SENSOR
          case SENSOR_TYPE_SYSTEM_STATUS:
        	  pSensor = new tSystemStatusSensor(SensorID);
              break;
    #endif //CONFIG_SYSTEM_STATUS_SENSOR
	#if CONFIG_WIEGAND_SENSOR
          case SENSOR_TYPE_WIEGAND:
        	  pSensor = new tWiegandSensor(SensorID);
              break;
	#endif //CONFIG_WIEGAND_SENSOR
          default:
        	  pSensor = appSpecificCreateSensor(SensorType, SensorID);
    }

    pSensor->setName(pName);
    return pSensor;
}

#endif //CONFIG_SENSORS
