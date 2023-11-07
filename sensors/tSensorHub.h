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

class tSensorDesc;

class tSensorHubEvent
{
public:
   tSensorHubEvent() {}
   virtual void onEvent(uint8_t SensorID, tSensorEventType EventType, uint8_t dataBlobSize, void *pDataBlob) = 0;

private:
   void Connect(tSensorHubEvent **pFirst) { pNext = *pFirst; *pFirst = this; }
   tSensorHubEvent *pNext; friend class tSensorHub;
};

/**
 * Sensor hub is an entity working on a central node, aggregating all sensor that may be on remote nodes
 *
 * As the sensors may be on remote nodes, the app has no direct access to tSensor class.
 */
class tSensorHub {
public:

	tSensorHub() { Instance = this; }
	static tSensorHub *Instance;

	uint8_t RegisterLocalSensor(uint8_t SensorID, char * pSensorName, uint8_t api_version);
	uint8_t RegisterRemoteSensor(uint8_t SensorID, char * pSensorName) {}

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

	/*
	 * Subscribe to all events from the sensor
	 * There's no unsubscribe - not needed in fact
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 * @retval SENSOR_STATUS_TIMEOUT
	 */
	uint8_t subscribeToEvents(uint8_t SensorID, tSensorHubEvent *pSensorEvent);

	/**
	 * Asyn get current sensor data from remote node
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 * @retval SENSOR_STATUS_TIMEOUT
	 */
	void getCurrentSensorDataRequest(uint8_t SensorID);
	virtual void getCurrentSensorDataResponse(uint8_t SensorID, uint8_t dataBlobSize, void *pDataBlob) {}

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

protected:
   /**
    * Application callback for app specific sensors factory, called when sensor has not been found by SensorDescFactory
    */
   virtual tSensorDesc *appSpecificSenorDescFactory(uint8_t SensorType) {}
#endif // CONFIG_SENSORS_JSON_OUTPUT

public:
   /*
    * to be called on sensor event, either remote or local
    */
   void onSensorEvent(uint8_t SensorID, tSensorEventType EventType, uint8_t dataBlobSize, void *pDataBlob);

private:
   /*
    * create a sensorDesc object based on sensor type
    */
   tSensorDesc *sensorDescFactory(uint8_t SensorType, uint8_t SensorID, char * pSensorName, uint8_t apiVersion);

   void callAllCallbacks(tSensorDesc *pSensorDesc, tSensorEventType EventType);

};

#endif //CONFIG_SENSOR_HUB
