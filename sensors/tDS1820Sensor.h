/*
 * tDS1820Sensor.h
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#ifndef SRC_TDS1820SENSOR_H_
#define SRC_TDS1820SENSOR_H_

#include "../../../global.h"
#include "tSensor.h"

class DallasTemperature;
typedef uint8_t DeviceAddress[8];

class tDS1820Sensor: public tSensor {
public:
   static const uint8_t MAX_DS1820_DEVICES_ON_BUS = 8;
   typedef struct
   {
      uint8_t Pin;   // pin the oneWire bus is connected to
      uint8_t NumOfDevices; // desired number of DS1820 devices, init will fail if other number is found on the wire
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
         //!!! FIX! real num of devices
   } tResult;

   tDS1820Sensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_DS1820,sensorID) {}

   virtual uint8_t SetSpecificConfig(void *pBlob);

   static uint8_t TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream);

   static void printAddress(uint8_t* pDeviceAddress, Stream *pStream);

   tResult *getCurrentMeasurement() { return (tResult *)mCurrentMeasurementBlob;}

protected:
   virtual void doTriggerMeasurement();
   virtual void doTimeTick();

private:
   static const uint8_t NUM_TICKS_TO_MEASURE_COMPETE = (750 / SENSOR_PROCESS_SERVICE_TIME)+1;
   uint8_t mAvg;
   uint8_t mNumOfDevices;
   uint8_t mTicksToMeasurementCompete;
   DallasTemperature *pDs1820;
};

#endif /* SRC_TDS1820SENSOR_H_ */
