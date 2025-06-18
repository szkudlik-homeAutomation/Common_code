/*
 * tDS1820Sensor.h
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"
#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorLogger.h"

#if CONFIG_DS1820_SENSOR_JSON_OUTPUT
uint8_t DS1820SensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache);
#endif //CONFIG_DS1820_SENSOR_JSON_OUTPUT

#if CONFIG_DS1820_SENSOR || CONFIG_DS1820_SENSOR_JSON_OUTPUT

class tDS1820SensorTypes {
public:
	   typedef uint8_t DeviceAddress[8];

	   typedef struct
	   {
	      DeviceAddress Addr;
	      int16_t Temperature;        // in celcius, 1 decimal position
	   } tDs1820Data;

	   typedef struct
	   {
	      uint8_t NumOfDevices;
	      uint8_t Avg;
	      tDs1820Data Dev[0];
	                     // size of the array is 1 if Avg = 1
	                     // or NumOfDevices if Avg = 0
	   } tResult_api_v1;

	   typedef struct
	   {
	      uint8_t Pin;   // pin the oneWire bus is connected to
	      uint8_t Avg;   // if true, measurements from all valid devices on the wire will be taken as average
	   } tConfig_api_v1;

	   static void printAddress(uint8_t* pDeviceAddress, Stream *pStream);
};
#endif


#if CONFIG_DS1820_SENSOR

class DallasTemperature;

class tDS1820Sensor : public tSensor, public tDS1820SensorTypes {
public:
   static const uint8_t MAX_DS1820_DEVICES_ON_BUS = 8;
   static const uint8_t DS1820_INVALID_ID = 0xFF;

   static const uint8_t API_VERSION = 1;
   typedef tConfig_api_v1 tConfig;
   typedef tResult_api_v1 tResult;

   /* the config */
   tConfig Config;


   tDS1820Sensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_DS1820, sensorID, API_VERSION, sizeof(Config), &Config) {}

   tResult *getCurrentMeasurement() { return (tResult *)mCurrentMeasurementBlob;}
   uint8_t compareAddr(uint8_t* pDeviceAddress1, uint8_t* pDeviceAddress2);
   uint8_t findDevID(uint8_t* pDeviceAddress);

protected:
   virtual void doTriggerMeasurement();
   virtual void doTimeTick();
   virtual uint8_t onSetConfig();

private:
   static const uint8_t NUM_TICKS_TO_MEASURE_COMPETE = (750 / SENSOR_PROCESS_SERVICE_TIME)+1;
   uint8_t mNumOfDevices;
   uint8_t mTicksToMeasurementCompete;
   DallasTemperature *pDs1820;

   bool isTempValid(int16_t temp) { return ((temp > -1200) && (temp < 799)); }
};
#endif // CONFIG_DS1820_SENSOR

#if CONFIG_SENSOR_LOGGER
class tDS1820SensorLogger : public tSensorLogger
{
public:
    tDS1820SensorLogger() : tSensorLogger(SENSOR_TYPE_DS1820, 0) {}
    tDS1820SensorLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_DS1820, sensorID) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob);
};
#endif CONFIG_SENSOR_LOGGER
