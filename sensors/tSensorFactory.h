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

#if CONFIG_SENSORS_FACTORY


class tSensorFactory {
public:
	static tSensorFactory* Instance;

	tSensorFactory() { Instance = this; };
	virtual ~tSensorFactory() { Instance = NULL; }

	/* create a sensor. Don' initialize, configure nor register in sensor hub */
	tSensor *CreateSensor(uint8_t SensorType, uint8_t SensorID);

	/* create a sensor. Do initialize, configure and register in sensor hub (if present)
	 * pSensorName is a name the sensor will be regitered in sensor hub, ignored if there's no sensor hub in the system
	 * pSensorName a nem of the sensor in PROGMEM
	 * pConfigBlob will be copied to sensor internals
	 * autoStart if true the sensor will start immediately, otherwise a pSensor->start() must be called
	 * serialEventMask a mask of events that sensor will sent over serial communication
	 */
	tSensor *CreateSensor(uint8_t SensorType, uint8_t SensorID, const __FlashStringHelper *pSensorName, uint8_t ApiVersion, void *pConfigBlob,
					      uint8_t configBlobSize, uint16_t measurementPeriod, bool autoStart, uint8_t serialEventMask);

protected:
	virtual tSensor *appSpecificCreateSensor(uint8_t SensorType, uint8_t SensorID) { return NULL; }

};

#endif //CONFIG_SENSORS_FACTORY
