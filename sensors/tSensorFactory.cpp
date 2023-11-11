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
#include "tSensorDesc.h"


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


#if CONFIG_SENSOR_HUB

static tSensorFactory* tSensorFactory::Instance;

tSensorDesc *tSensorFactory::CreateDesc(uint8_t SensorType, uint8_t SensorID, char * pSensorName, uint8_t apiVersion)
{
    tSensorDesc *newSensorDesc = NULL;
    switch (SensorType)
    {
    #if CONFIG_DS1820_SENSOR
          case SENSOR_TYPE_DS1820:
              newSensorDesc = new tDs1820SensorDesc();
              break;
    #endif // CONFIG_DS1820_SENSOR

    #if CONFIG_IMPULSE_SENSOR
          case SENSOR_TYPE_IMPULSE:
             newSensorDesc = new tImpulseSensorDesc();
             break;
    #endif //CONFIG_IMPULSE_SENSOR

    #if CONFIG_PT100_ANALOG_SENSOR
             case SENSOR_TYPE_PT100_ANALOG:
                 newSensorDesc = new tPt100AnalogSensorDesc();
                 break;
    #endif

    #if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
          case SENSOR_TYPE_DIGITAL_INPUT:
             newSensorDesc = new tSimpleDigitalInputSensorDesc();
             break;
    #endif

    #if CONFIG_OUTPUT_STATE_SENSOR
         case SENSOR_TYPE_OUTPUT_STATES:
             newSensorDesc = new tOutputStateSensorDesc();
             break;
    #endif

    #if CONFIG_SYSTEM_STATUS_SENSOR
          case SENSOR_TYPE_SYSTEM_STATUS:
              newSensorDesc = new tSystemStatusSensorDesc();
              break;
    #endif //CONFIG_SYSTEM_STATUS_SENSOR

          default:
              newSensorDesc = appSpecificCreateDesc(SensorType);
    }

    if (NULL == newSensorDesc)
        newSensorDesc = new tSensorDesc(); /* generic, no JSON output */

    newSensorDesc->sensorApiVersion = apiVersion;
    newSensorDesc->sensorType = SensorType;
    newSensorDesc->SensorID = SensorID;
    newSensorDesc->pName = pSensorName;

    return newSensorDesc;
}
#endif // CONFIG_SENSOR_HUB

tSensor *tSensorFactory::CreateSensor(uint8_t SensorType, uint8_t SensorID, uint8_t ApiVersion, void *pConfigBlob,
	      uint8_t configBlobSize, uint16_t measurementPeriod, bool autoStart)
{
	DEBUG_PRINT_3("Creating sensor type ");
	DEBUG_3(print(SensorType,DEC));
	DEBUG_PRINT_3(" ID ");
	DEBUG_3(print(SensorID,DEC));
	tSensor *pSensor = CreateSensor(SensorType);
	if (NULL == pSensor)
	{
		DEBUG_PRINTLN_3(" error: cannot create sensor");
		return NULL;
	}

	if ((ApiVersion != pSensor->getSensorApiVersion()) ||
		 configBlobSize != pSensor->getConfigBlobSize())
	{
		DEBUG_PRINTLN_3(" error: api version mismatch");
		goto free;
	}

	pSensor->setConfig(SensorID, measurementPeriod, pConfigBlob);
	if(autoStart)
		pSensor->Start();

	return pSensor;

free:
	delete(pSensor);
	return NULL;
}

tSensor *tSensorFactory::CreateSensor(uint8_t SensorType)
{
	tSensor *pSensor = NULL;
    switch (SensorType)
    {
    #if CONFIG_DS1820_SENSOR
          case SENSOR_TYPE_DS1820:
        	  pSensor = new tDS1820Sensor();
              break;
    #endif // CONFIG_DS1820_SENSOR

    #if CONFIG_IMPULSE_SENSOR
          case SENSOR_TYPE_IMPULSE:
        	 pSensor = new tImpulseSensor();
             break;
    #endif //CONFIG_IMPULSE_SENSOR

    #if CONFIG_PT100_ANALOG_SENSOR
          case SENSOR_TYPE_PT100_ANALOG:
        	  pSensor = new tPt100AnalogSensor();
              break;
    #endif

    #if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
          case SENSOR_TYPE_DIGITAL_INPUT:
        	  pSensor = new tSimpleDigitalInputSensor();
              break;
    #endif

    #if CONFIG_OUTPUT_STATE_SENSOR
         case SENSOR_TYPE_OUTPUT_STATES:
        	 pSensor = new tOutputStateSensor();
             break;
    #endif

    #if CONFIG_SYSTEM_STATUS_SENSOR
          case SENSOR_TYPE_SYSTEM_STATUS:
        	  pSensor = new tSystemStatusSensor();
              break;
    #endif //CONFIG_SYSTEM_STATUS_SENSOR

          default:
        	  pSensor = appSpecificCreateSensor(SensorType);
    }

    return pSensor;
}

#endif //CONFIG_SENSORS
