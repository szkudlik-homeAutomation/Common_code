/*
 * tSensorFactory.h
 *
 *  Created on: 11 lis 2023
 *      Author: szkud
 */
#pragma once

#include "../../../global.h"

class tSensor;
#include "tSensorCache.h"

#if CONFIG_SENSORS


class tSensorFactory {
public:
	static tSensorFactory* Instance;

	tSensorFactory() { Instance = this; };
	virtual ~tSensorFactory() { Instance = NULL; }

	tSensor *CreateSensor(uint8_t SensorType, uint8_t SensorID);
	tSensor *CreateSensor(uint8_t SensorType, uint8_t SensorID, uint8_t ApiVersion, void *pConfigBlob,
					      uint8_t configBlobSize, uint16_t measurementPeriod, bool autoStart);

#if CONFIG_SENSORS_JSON_OUTPUT
   /* get a function pointer to JSON encoder for given sensor type and API version. Null if no matching function found */
   doFormatJSON getJSONFormatFunction(uint8_t SensorType, uint8_t apiVersion);
#endif // CONFIG_SENSORS_JSON_OUTPUT

protected:
	virtual tSensor *appSpecificCreateSensor(uint8_t SensorType, uint8_t SensorID) { return NULL; }

#if CONFIG_SENSOR_HUB
	/**
    * Application callback for app specific sensors factory, called when sensor has not been found by SensorCacheFactory
    */
   virtual doFormatJSON  appSpecificGetSJONFrormatFunction(uint8_t SensorType, uint8_t apiVersion) { return NULL; }
#endif CONFIG_SENSOR_HUB

};

#endif //CONFIG_SENSORS
