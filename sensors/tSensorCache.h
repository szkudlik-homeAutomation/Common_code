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
public:
   tSensorCache() :
	   pDataCache(NULL),
	   pRemoteDataCache(NULL),
	   mFormatJSON(NULL),
	   mDataBlobSize(0),
	   Status(STATUS_NO_DATA_RECIEVED)
   {
	   pNext = pFirst; pFirst = this;
	   resetTimestamp();
   }

   uint8_t Status;
   uint8_t SensorID;
   uint8_t sensorType;
private:
   uint8_t mDataBlobSize;
public:
   uint8_t getDataBlobSize() const { return mDataBlobSize; }
   uint8_t sensorApiVersion;
   uint8_t mNodeID;     // id of a node the sensor is located on. 0 => local sensor
   void *pDataCache;
   void *pRemoteDataCache;  // pointer to additional data cache used for assembling incoming data (if needed)
   uint8_t mSeq;    // packet reassembly seq
   char * pName;

   uint8_t setDataBlobSize(uint8_t dataBlobSize);

	/*
	 * Format cached data as JSON
	 *
	 * note! format JSON should support all APIs that are in the system (backward compatibility)
	 */
   uint8_t formatJSON(Stream *pStream);

   void resetTimestamp() { mLastTimestamp = millis();}
   uint16_t getTimeSinceUpdate() { uint32_t diff = millis() - mLastTimestamp; return diff / 1000; }

   static tSensorCache *getFirst() { return pFirst; }
   tSensorCache *getNext() { return pNext; }

   static tSensorCache *getByID(uint8_t SensorID);
   static tSensorCache *getByName(const char * pSensorName);

   /* C-style function pointer, no point for a class and virtual method here */
   doFormatJSON mFormatJSON;
private:
   tSensorCache *pNext;
   static tSensorCache *pFirst;
   uint32_t mLastTimestamp;	// millis()
};

#endif // CONFIG_SENSOR_HUB
