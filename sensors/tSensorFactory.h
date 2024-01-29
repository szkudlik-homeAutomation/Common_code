/*
 * tSensorFactory.h
 *
 *  Created on: 11 lis 2023
 *      Author: szkud
 */
#pragma once

#include "../../../global.h"

class tSensor;
class tSensorDesc;

#if CONFIG_SENSORS


class tSensorFactory {
public:
	static tSensorFactory* Instance;

	tSensorFactory() { Instance = this; };
	virtual ~tSensorFactory() { Instance = NULL; }

	tSensor *CreateSensor(uint8_t SensorType, uint8_t SensorID);
	tSensor *CreateSensor(uint8_t SensorType, uint8_t SensorID, uint8_t ApiVersion, void *pConfigBlob,
					      uint8_t configBlobSize, uint16_t measurementPeriod, bool autoStart);

#if CONFIG_SENSOR_HUB
   /*
	* create a sensorDesc object based on sensor type
	* nodeID - id of a node this sensor is located on. 0 => local sensor
	*/
   tSensorDesc *CreateDesc(uint8_t SensorType, uint8_t SensorID, char * pSensorName, uint8_t apiVersion, uint8_t dataBlobSize, uint8_t nodeID);

#endif CONFIG_SENSOR_HUB

protected:
	virtual tSensor *appSpecificCreateSensor(uint8_t SensorType, uint8_t SensorID) { return NULL; }

#if CONFIG_SENSOR_HUB
	/**
    * Application callback for app specific sensors factory, called when sensor has not been found by SensorDescFactory
    */
   virtual tSensorDesc *appSpecificCreateDesc(uint8_t SensorType) { return NULL; }
#endif CONFIG_SENSOR_HUB

};

#endif //CONFIG_SENSORS
