/*
 * tSensorCache.h
 *
 *  Created on: Oct 4, 2023
 *      Author: mszkudli
 */

#pragma once
#include "../../../global.h"

#if CONFIG_SENSOR_HUB


/**
 * tSensorCache is an instance describing a sensor that may run on a different node
 * It is used by SensorHub to keep track and present data from all sensors in the
 *
 * tSensorCache keeps a copy (cache) of current data produced by the sensor
 */
class tSensorCache;
#include "tSensorJsonOutput.h"

class tSensorCache
{
private:
    int8_t mState;
    uint8_t mSensorID;
    uint8_t mSensorType;
    uint8_t mDataBlobSize;
    uint8_t mSensorApiVersion;
    uint8_t mNodeID;     // id of a node the sensor is located on. 0 => local sensor
    uint16_t mMeasurementPeriod;
    char * mName;
    void *pDataCache;
#if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
    void *pRemoteDataCache;  // pointer to additional data cache used for assembling incoming data (if needed)
    uint8_t mSeq;    // packet reassembly seq
#endif CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
    /* C-style function pointer, no point for a class and virtual method here */
#if CONFIG_SENSORS_JSON_OUTPUT
    tSensorJsonFormatter *mFormatJSON;
#endif //CONFIG_SENSORS_JSON_OUTPUT
    uint32_t mLastTimestamp;	// millis()

    uint8_t setDataBlobSize(uint8_t dataBlobSize);
    void resetTimestamp() { mLastTimestamp = millis(); UpdateTimeout(); }

public:
	// >0 - working states
    static const int8_t state_not_seen = 0;		// entry created with name, but the seneor has not yet been seen
    static const int8_t state_not_configured = 1; // seen, but config not set yet
	static const int8_t state_no_data_recieved = 2;		// dected, all metadata set, no payload data seen yet
	static const int8_t state_working = 3;				//
	static const int8_t state_timeout = 4;
	static const int8_t state_sensor_error_reported = 5;
	static const int8_t state_data_transfer_error = 6;

	// <0 are non-recoverable errors
	static const int8_t state_create_error = -1;
	static const int8_t state_incorrect_data_size = -2;
	static const int8_t state_inconsistent_params = -3;

    tSensorCache(uint8_t ID, uint8_t NodeID) :
	   mSensorID(ID),
	   mNodeID(NodeID),
	   mName(NULL),
	   pDataCache(NULL),
#if CONFIG_SENSORS_JSON_OUTPUT
	   mFormatJSON(NULL),
#endif //CONFIG_SENSORS_JSON_OUTPUT
	   mDataBlobSize(0),
	   mState(state_not_seen)
#if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
	   ,
	   pRemoteDataCache(NULL),
	   mSeq(0)
#endif CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
   {
	   pNext = pFirst; pFirst = this;
	   resetTimestamp();
   }

   void setError(int8_t errorState)
   {
       resetTimestamp();
       if (mState >=0)
           {
           mState = errorState;
           }
   }
   bool isDetected() const { return mState > state_not_seen; }
   bool isWorkingState() const { return mState > state_no_data_recieved; }
   bool isWorkingOrReadyState() const { return mState >= state_no_data_recieved; }

   bool isTimeout() const { return mState == state_timeout; }
   bool isConfigured() const { return mState > state_not_configured; }
   bool isPermanentError() const { return mState < 0; }
   bool isLocalSensor() const { return mNodeID == 0; }

#if CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS
   void resetDataSegment() {mSeq = 0;}
   void *getAssembledData() { return pRemoteDataCache; }
   uint8_t addDataSegment(uint8_t SegmentSeq, void *Payload);
   bool isDataAssemblyNeeded() const { return pRemoteDataCache != NULL; }
#endif	//CONFIG_SENSOR_HUB_FOR_REMOTE_SENSORS

   uint8_t getSensorType() const { return mSensorType; }
   uint8_t getSensorApiVersion() const { return mSensorApiVersion; }
   uint8_t getNodeID() const { return mNodeID; }
   void UpdateTimeout();
   uint8_t setData(void *dataSrc, uint8_t dataSize);
   char * GetName() { return mName; }
   uint8_t GetSensorID() const { return mSensorID; }


   // set a name based on a given progmem offset
   uint8_t setNameProgmem(const __FlashStringHelper *pName);

   // set a name from eeprom from given Eeprom offset
   //TODO
   //uint8_t setNameEeprom(uint16_t offset, uint8_t len);

   // generate a unique name based on sensor ID
   uint8_t generateName();

   uint8_t setAsDetected();

   //measurementPeriod in 0.1s
   uint8_t setParams(uint8_t SensorType, uint8_t ApiVersion, uint8_t dataBlobSize, uint16_t measurementPeriod);
   uint8_t getDataBlobSize() const { return mDataBlobSize; }
   void *getData() { return pDataCache; }

#if CONFIG_SENSORS_JSON_OUTPUT
	/*
	 * Format cached data as JSON
	 *
	 * note! format JSON should support all APIs that are in the system (backward compatibility)
	 */
   uint8_t formatJSON(Stream *pStream);
#endif //CONFIG_SENSORS_JSON_OUTPUT

   // time sice last sensor update in 1/10h of seconds
   uint16_t getTimeSinceUpdate() { uint32_t diff = millis() - mLastTimestamp; return diff / 100; }

   static tSensorCache *getByID(uint8_t SensorID, uint8_t deviceID);
   static tSensorCache *getByName(const char * pSensorName);


   static tSensorCache *getFirst() { return pFirst; }
   tSensorCache *getNext() { return pNext; }

private:
   tSensorCache *pNext;
   static tSensorCache *pFirst;
};

#endif // CONFIG_SENSOR_HUB
