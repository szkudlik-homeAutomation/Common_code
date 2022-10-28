/*
 * tSensorHub.h
 *
 *  Created on: Oct 28, 2022
 *      Author: mszkudli
 */

#ifndef SRC_COMMON_CODE_SENSORS_TSENSORHUB_H_
#define SRC_COMMON_CODE_SENSORS_TSENSORHUB_H_

#include "../../../global.h"

class tSensorHubEvent
{
public:
   typedef enum
   {
      EV_TYPE_MEASUREMENT_ERROR,
      EV_TYPE_MEASUREMENT_COMPLETED,
      EV_TYPE_MEASUREMENT_CHANGE,
      EV_TYPE_THOLD_EXCEEDED
   } tEventType;


   tSensorHubEvent() : mpNext(NULL) {}
   virtual ~tSensorHubEvent() {}

   virtual void onEvent(uint8_t SensorID, tEventType EventType, uint8_t dataBlobSize, void *pDataBlob) = 0;
private:
   tSensorHubEvent *mpNext; friend class tSensorHub;
};


/**
 * Sensor hub is an entity working on a central node, aggregating all sensor that may be on remote nodes
 *
 * As the sensors may be on remote nodes, the app has no direct access to tSensor class.
 *
 * Access to the sensor goes either directly or using CREATE_SENSOR etc. messages
 */
class tSensorHub {
public:
	tSensorHub();

	/*
	 * Send a create sensor message / create a local sensor
	 * virtual CreateSensorResponse will be called when operation is done
	 *
	 */
	void CreateSensorRequest(uint8_t TargetNode, uint8_t SensorType, uint8_t sensorID, char * pSensorName);
	/*
	 * @retval 0 OK
	 * @retval SENSOR_STATUS_DUPLICATE_ID
	 * @retval SENSOR_STATUS_DUPLICATE_NAME
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 * @retval SENSOR_STATUS_TIMEOUT
	 */
	virtual uint8_t CreateSensorResponse(uint8_t sensorID) {};

	/*
	 * Get an ID of a sensor by name
	 * Immediate response, local operation
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 * or sensor ID
	 */
	uint8_t getSensorID(char * pSensorName) const;

	/*
	 * Get an ID of a sensor info
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 * @retval SENSOR_STATUS_TIMEOUT
	 */
	void getSensorInfoRequest(uint8_t SensorID);
	virtual uint8_t getSensorInfoResponse(
			uint8_t SensorID,
			uint8_t DeviceId,
			uint8_t SensorType,
			bool isRunning,
			uint16_t MeasurementPeriod,
			const char *sensorName) {};

	/*
	 * Subscribe to all events from the sensor
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 * @retval SENSOR_STATUS_TIMEOUT
	 */
	uint8_t subscribeToEvents(uint8_t SensorId, tSensorHubEvent *pSensorEvent);
	uint8_t unSubscribeToEvents(uint8_t SensorId);

	/**
	 * Asyn get current sensor data
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 * @retval SENSOR_STATUS_TIMEOUT
	 */
	void getCurrentSensorDataRequest(uint8_t SensorID);
	void getCurrentSensorDataResponse(uint8_t SensorID, uint8_t dataBlobSize, void *pDataBlob);

	/**
	 * get cached sensor data
	 * get data from a sensor stored locally
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 */
	uint8_t getCachedSensorData(uint8_t SensorID,  uint8_t *dataBlobSize, void **pDataBlob);

	/**
	 * get cached sensor data as JSON
	 * get data from a sensor stored locally, formatted in JSON, and stream them to provided (tcp)stream
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 */
	uint8_t getCachedSensorDataJson(uint8_t SensorID, Stream *pStream);

};

#endif /* SRC_COMMON_CODE_SENSORS_TSENSORHUB_H_ */
