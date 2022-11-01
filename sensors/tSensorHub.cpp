/*
 * tSensorHub.cpp
 *
 *  Created on: Oct 28, 2022
 *      Author: mszkudli
 */

#include "tSensorHub.h"

tSensorHub::tSensorHub() {
	// TODO Auto-generated constructor stub

}

uint8_t tSensorHub::getCachedSensorDataJson(uint8_t SensorID, Stream *pStream)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return SENSOR_NOT_FOUND;
   }
   // note that the sensor may be located on a remote machine, use cached data
   return tSensor::TranslateBlobToJSON(pSensorDesc->sensorType, pSensorDesc->dataBlobSize, pSensorDesc->pDataCache, pStream);
}
