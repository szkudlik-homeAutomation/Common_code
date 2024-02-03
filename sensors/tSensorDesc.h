/*
 * tSensorDesc1.h
 *
 *  Created on: Oct 4, 2023
 *      Author: mszkudli
 */

#pragma once
#include "../../../global.h"

#if CONFIG_SENSOR_HUB


/**
 * tSensorDesc is an instance describing a sensor that may run on a diffrent node
 * Each sensor in whole system (including ones on master nore) must have corresponding \
 * SensorDesc on master node
 *
 * Sensor desc keeps a copy (cache) of current data produced by the sensor
 */
class tSensorDesc
{
public:
   tSensorDesc() :
	   pDataCache(NULL),
	   pRemoteDataCache(NULL),
	   mDataBlobSize(0),
	   Status(STATUS_NO_DATA_RECIEVED)
   {
	   pNext = pFirst; pFirst = this;
	   resetTimestamp();
   }

   uint8_t Status;
   uint8_t SensorID;
   uint8_t sensorType;
   uint8_t mDataBlobSize;
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

   static tSensorDesc *getFirst() { return pFirst; }
   tSensorDesc *getNext() { return pNext; }

   static tSensorDesc *getByID(uint8_t SensorID);
   static tSensorDesc *getByName(const char * pSensorName);

protected:
   /* sensor specific JSON formatter */
    virtual uint8_t doFormatJSON(Stream *pStream) {}
private:
   tSensorDesc *pNext;
   static tSensorDesc *pFirst;
   uint32_t mLastTimestamp;	// millis()
};

#endif // CONFIG_SENSOR_HUB
