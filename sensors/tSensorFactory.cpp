/*
 * tSensorFactory.cpp
 *
 *  Created on: 11 lis 2023
 *      Author: szkud
 */


#include "../../../global.h"
#if CONFIG_SENSORS_FACTORY

#include "tSensorFactory.h"
#include "tSensor.h"
#include "tSensorHub.h"

#include "tDS1820Sensor.h"
#include "tImpulseSensor.h"
#include "tPt100AnalogSensor.h"
#include "tSimpleDigitalInputSensor.h"
#include "tOutputStateSensor.h"
#include "tSystemStatusSensor.h"
#include "tWiegandSensor.h"
#include "tSht3Sensor.h"
#include "tOutputStateSensor.h"
#include "tTgs2603AnalogSensor.h"

static tSensorFactory* tSensorFactory::Instance;

tSensor *tSensorFactory::CreateSensor(uint8_t SensorType, uint8_t SensorID,
		  const __FlashStringHelper *pSensorName, uint8_t ApiVersion, void *pConfigBlob,
	      uint8_t configBlobSize, uint16_t measurementPeriod, bool autoStart, uint8_t serialEventMask)
{
	uint8_t Status;
	tSensor *pSensor = CreateSensor(SensorType, SensorID);
	if (NULL == pSensor)
	{
		DEBUG_PRINTLN_3(" error: cannot create sensor");
		return NULL;
	}

	Status = pSensor->setConfig(measurementPeriod, ApiVersion, pConfigBlob, configBlobSize);
	if (STATUS_SUCCESS != Status)
	{
		// sensor cannot be delted, this situation is more/less fatal
		DEBUG_PRINTLN_3(" error: cannot set sensor config");
		return NULL;
	}

	pSensor->setSensorSerialEventsMask(serialEventMask);

	if(autoStart)
		pSensor->Start();

#if CONFIG_SENSOR_HUB
#if CONFIG_REMOTE_SENSORS_TEST
    if (SensorID == 1)
    	//sensors with ID > 1 won't be registered in sensorHub locally
#endif // CONFIG_REMOTE_SENSORS_TEST
    	tSensorHub::Instance->RegisterSensor(SensorID, 0, pSensorName);
#endif // CONFIG_SENSOR_HUB
	return pSensor;
}

tSensor *tSensorFactory::CreateSensor(uint8_t SensorType, uint8_t SensorID)
{
	DEBUG_PRINT_3("Creating sensor type ");
	DEBUG_3(print(SensorType,DEC));
	DEBUG_PRINT_3(" ID ");
	DEBUG_3(println(SensorID,DEC));

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
    #if CONFIG_SHT3_DIS_SENSOR
          case SENSOR_TYPE_SHT3:
        	  pSensor = new tSht3Sensor(SensorID);
        	  break;
    #endif //CONFIG_SHT3_DIS_SENSOR
	#if CONFIG_TGS2603_ODOUR_SENSOR
          case SENSOR_TYPE_TGS2603:
        	  pSensor = new tTgs2603AnalogSensor(SensorID);
    	  break;
	#endif
          default:
        	  pSensor = appSpecificCreateSensor(SensorType, SensorID);
    }

    return pSensor;
}

#if CONFIG_SENSORS_FACTORY_INSTANCE
tSensorFactory SensorFactory;
#endif // CONFIG_SENSORS_FACTORY_INSTANCE

#endif // CONFIG_SENSORS_FACTORY
