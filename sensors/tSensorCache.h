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
 * system (from all nodes)
 *
 * tSensorCache keeps a copy (cache) of current data produced by the sensor
 */
class tSensorCache;
typedef uint8_t (*doFormatJSON)(Stream *pStream, tSensorCache *cache);

class tSensorCache
{
private:
    int8_t mState;
    uint8_t mSensorID;
    uint8_t mSensorType;
    uint8_t mDataBlobSize;
    uint8_t mSensorApiVersion;
    uint8_t mNodeID;     // id of a node the sensor is located on. 0 => local sensor
    char * pName;
    void *pDataCache;
    void *pRemoteDataCache;  // pointer to additional data cache used for assembling incoming data (if needed)
    uint8_t mSeq;    // packet reassembly seq
    /* C-style function pointer, no point for a class and virtual method here */
    doFormatJSON mFormatJSON;
    uint32_t mLastTimestamp;	// millis()

    uint8_t setDataBlobSize(uint8_t dataBlobSize);
    void resetTimestamp() { mLastTimestamp = millis();}

public:
	static const int8_t state_not_detected  = 1;		// created, not seen in the system yet, not all metadata set
	static const int8_t state_no_data_recieved = 2;		// dected, all metadata set, no payload data seen yet
	static const int8_t state_working = 3;				//
	static const int8_t state_timeout = 4;
	static const int8_t state_sensor_error_reported = -1;
	static const int8_t state_create_error = -2;
	static const int8_t state_incorrect_data_size = -3;

    tSensorCache(char *name, uint8_t ID) :
	   mSensorID(ID),
	   pName(name),
	   pDataCache(NULL),
	   pRemoteDataCache(NULL),
	   mFormatJSON(NULL),
	   mDataBlobSize(0),
	   mState(state_not_detected)
   {
	   pNext = pFirst; pFirst = this;
	   resetTimestamp();
   }

   void setError(int8_t errorState) { resetTimestamp(); if (errorState < 0) mState = errorState; }
   uint8_t setData(void *dataSrc, uint8_t dataSize);
   char * GetName() { return pName; }
   uint8_t GetSensorID() const { return mSensorID; }
   uint8_t setParams(uint8_t SensorType, uint8_t ApiVersion, uint8_t nodeID, uint8_t dataBlobSize);
   uint8_t getDataBlobSize() const { return mDataBlobSize; }
   void *getData() { return pDataCache; }

	/*
	 * Format cached data as JSON
	 *
	 * note! format JSON should support all APIs that are in the system (backward compatibility)
	 */
   uint8_t formatJSON(Stream *pStream);

   uint16_t getTimeSinceUpdate() { uint32_t diff = millis() - mLastTimestamp; return diff / 1000; }

   static tSensorCache *getByID(uint8_t SensorID);
   static tSensorCache *getByName(const char * pSensorName);


   static tSensorCache *getFirst() { return pFirst; }
   tSensorCache *getNext() { return pNext; }

private:
   tSensorCache *pNext;
   static tSensorCache *pFirst;
};

#endif // CONFIG_SENSOR_HUB
