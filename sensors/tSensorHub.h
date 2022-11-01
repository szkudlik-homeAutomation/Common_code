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


   tSensorHubEvent() {}
   virtual void onEvent(uint8_t SensorID, tEventType EventType, uint8_t dataBlobSize, void *pDataBlob) = 0;
private:
   void Connect(tSensorHubEvent **pFirst) { pNext = *pFirst; *pFirst = this; }
   tSensorHubEvent *pNext; friend class tSensorHub;
};


class tSensorDesc
{
public:
   tSensorDesc() { pNext = pFirst; pFirst = this; }

   uint8_t SensorID;
   uint8_t sensorType;
   uint8_t dataBlobSize;
   void *pDataCache;
   char * pName;
   tSensorHubEvent *pFirstEventHander;

   tSensorDesc *getFirst() { return pFirst; }
   tSensorDesc *getNext() { return pNext; }

   static tSensorDesc *getByID(uint8_t SensorID);
   static tSensorDesc *getByName(const char * pSensorName);
private:
   tSensorDesc *pNext;
   static tSensorDesc *pFirst;
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

	tSensorHub() {};

	/*
	 * Send a create sensor message / create a local sensor
	 * virtual CreateSensorResponse will be called when operation is done
	 *
	 * @param TargetNode (TBD) a remote target ID where the sensor is to be created
	 * @SensorType type of the sensor
	 * @SensorID unqique ID of the sensor
	 * @pSensorName poiner to sensor name in RAM (not in progmem), the name must be static, won't be copied to repo
	 * @pConfigBlob pointer to sensor specific config structure
	 * @MeasurementPeriod period of measurement (100ms unit)
	 */
	void CreateSensorRequest(uint8_t TargetNode, uint8_t SensorType, uint8_t SensorID, char * pSensorName, void* pConfigBlob, uint8_t MeasurementPeriod);

	/*
	 * Method called when a response to CreateSensorRequest arrives
	 * when sensor is on the same node, it may be called before CreateSensorRequest returns
	 *
	 * Status 0 OK
	 *  SENSOR_STATUS_DUPLICATE_ID
	 *  SENSOR_STATUS_DUPLICATE_NAME
	 *  SENSOR_STATUS_UNKNOWN_SENSOR
	 *  SENSOR_STATUS_TIMEOUT
	 *  CREATE_SENSOR_STATUS_CONFIG_SET_ERROR
	 */
	virtual void CreateSensorResponse(uint8_t SensorID, uint8_t Status) {};

	/*
	 * Get an ID of a sensor by name
	 * Immediate response, local operation
	 *
	 * @retval SENSOR_NOT_FOUND
	 * or sensor ID
	 */
	uint8_t getSensorID(const char * pSensorName);

	/*
	 * Get an ID of a sensor info (remote operation)
	 * Status: SENSOR_NOT_FOUND
	 *         SENSOR_STATUS_TIMEOUT
	 */
	void getSensorInfoRequest(uint8_t SensorID);
	virtual void getSensorInfoResponse(
	      uint8_t Status,
			uint8_t SensorID,
			uint8_t DeviceId,
			uint8_t SensorType,
			bool isRunning,
			uint16_t MeasurementPeriod,
			const char *sensorName) {};

	/*
	 * Subscribe to all events from the sensor
	 * There's no unsubscribe - not needed in fact
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 * @retval SENSOR_STATUS_TIMEOUT
	 */
	uint8_t subscribeToEvents(uint8_t SensorID, tSensorHubEvent *pSensorEvent);

	/**
	 * Asyn get current sensor data
	 *
	 * @retval SENSOR_STATUS_UNKNOWN_SENSOR
	 * @retval SENSOR_STATUS_TIMEOUT
	 */
	void getCurrentSensorDataRequest(uint8_t SensorID);
	virtual void getCurrentSensorDataResponse(uint8_t SensorID, uint8_t dataBlobSize, void *pDataBlob) {}

	/**
	 * get cached sensor data
	 * get data from a sensor stored locally
	 *
	 * @retval SENSOR_NOT_FOUND
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

extern tSensorHub SensorHub;

#endif /* SRC_COMMON_CODE_SENSORS_TSENSORHUB_H_ */
