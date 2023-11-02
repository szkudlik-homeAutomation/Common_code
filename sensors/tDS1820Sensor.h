/*
 * tDS1820Sensor.h
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#pragma once


#include "../../../global.h"
#if CONFIG_DS1820_SENSOR

#include "tSensor.h"
#include "tSensorDesc.h"

class DallasTemperature;

#if CONFIG_SENSORS_JSON_OUTPUT
#if !CONFIG_SENSOR_HUB
#error CONFIG_SENSORS_JSON_OUTPUT requires CONFIG_SENSOR_HUB
#endif
#endif

#if CONFIG_SENSOR_HUB

class tDs1820SensorDesc : public tSensorDesc
{
public:
	tDs1820SensorDesc(uint8_t aSensorID, char * apSensorName) :
		tSensorDesc(SENSOR_TYPE_DS1820, aSensorID, apSensorName) {}

protected:
#if CONFIG_SENSORS_JSON_OUTPUT
   /* sensor specific JSON formatter */
    virtual uint8_t doFormatJSON(Stream *pStream);
#endif // CONFIG_SENSORS_JSON_OUTPUT
};

#endif //CONFIG_SENSOR_HUB


class tDS1820Sensor: public tSensor {
public:
   static const uint8_t MAX_DS1820_DEVICES_ON_BUS = 8;
   static const uint8_t DS1820_INVALID_ID = 0xFF;

   typedef uint8_t DeviceAddress[8];
   typedef struct
   {
      uint8_t Pin;   // pin the oneWire bus is connected to
      uint8_t Avg;   // if true, measurements from all valid devices on the wire will be taken as average
   } tConfig;

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
   } tResult;

   /* the config */
   tConfig Config;

   static const uint8_t API_VERSION = 1;

   tDS1820Sensor() : tSensor(SENSOR_TYPE_DS1820, API_VERSION) {}

   static void printAddress(uint8_t* pDeviceAddress, Stream *pStream);


   tResult *getCurrentMeasurement() { return (tResult *)mCurrentMeasurementBlob;}
   uint8_t compareAddr(uint8_t* pDeviceAddress1, uint8_t* pDeviceAddress2);
   uint8_t findDevID(uint8_t* pDeviceAddress);

protected:
   virtual void doTriggerMeasurement();
   virtual void doTimeTick();
   virtual uint8_t doSetConfig();

private:
   static const uint8_t NUM_TICKS_TO_MEASURE_COMPETE = (750 / SENSOR_PROCESS_SERVICE_TIME)+1;
   uint8_t mNumOfDevices;
   uint8_t mTicksToMeasurementCompete;
   DallasTemperature *pDs1820;

   bool isTempValid(int16_t temp) { return ((temp > -1200) && (temp < 799)); }
};

#endif // CONFIG_DS1820_SENSOR
