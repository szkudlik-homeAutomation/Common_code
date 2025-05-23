/*
 * tSensorHub.h
 *
 *  Created on: Oct 28, 2022
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"
#if CONFIG_SENSOR_HUB

#include "tSensor.h"

/**
 * Sensor hub is an entity working on a central node, aggregating all sensor that may be on remote nodes
 *
 * As the sensors may be on remote nodes, the app has no direct access to tSensor class.
 */
class tSensorHub {
public:

	tSensorHub()
	{
	    Instance = this;
	}
	static tSensorHub *Instance;

	/* name from progmem */
	uint8_t RegisterSensor(uint8_t SensorID, const __FlashStringHelper *pSensorName);

	/*
	 * Get an ID of a sensor by name
	 * Immediate response, local operation
	 *
	 * @retval SENSOR_ID_NOT_FOUND
	 * or sensor ID
	 */
	uint8_t getSensorID(const char * pSensorName);

	/**
	 * return a pointer to sensor name or NULL if the sensor does not exist
	 */
	const char *getSensorName(uint8_t SensorID);

	/**
	 * get cached sensor data
	 * get data from a sensor stored locally
	 */
	uint8_t getCachedSensorData(uint8_t SensorID,  uint8_t *dataBlobSize, void **pDataBlob);

#if CONFIG_SENSORS_JSON_OUTPUT
	/**
	 * get cached sensor data as JSON
	 * get data from a sensor stored locally, formatted in JSON, and stream them to provided (tcp)stream
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 */
	uint8_t getCachedSensorDataJson(uint8_t SensorID, Stream *pStream);

   /**
    * get all sensor data as JSON
    * get data from a sensor stored locally, formatted in JSON, and stream them to provided (tcp)stream
    */
   uint8_t getCachedSensorsDataJson(Stream *pStream);

#endif // CONFIG_SENSORS_JSON_OUTPUT

public:
   /*
    * to be called on sensor event, either remote or local
    */
   void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob);
};

#endif //CONFIG_SENSOR_HUB
