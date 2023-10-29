/*
 * tSensorDesc1.h
 *
 *  Created on: Oct 4, 2023
 *      Author: mszkudli
 */

#pragma once
#include "../../../global.h"

class tSensorHubEvent;

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
   tSensorDesc(uint8_t aSensorType, uint8_t aSensorID, char * apSensorName) :
	   SensorID(aSensorID),
	   sensorType(aSensorType),
	   pName(apSensorName),
	   pDataCache(NULL),
	   pFirstEventHander(NULL),
	   dataBlobSize(0),
	   Status(STATUS_NO_DATA_RECIEVED)
   {
	   pNext = pFirst; pFirst = this;
   }

   uint8_t Status;
   uint8_t SensorID;
   uint8_t sensorType;
   uint8_t dataBlobSize;
   void *pDataCache;
   char * pName;
   tSensorHubEvent *pFirstEventHander;

	/*
	 * Format cached data as JSON
	 */
   uint8_t formatJSON(Stream *pStream);

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


};
