/*
 * tImpulseSensor.h
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"
#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorLogger.h"

#if CONFIG_IMPULSE_SENSOR_JSON_OUTPUT
uint8_t ImpulseSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache);
#endif //CONFIG_IMPULSE_SENSOR_JSON_OUTPUT

#if CONFIG_IMPULSE_SENSOR || CONFIG_IMPULSE_SENSOR_JSON_OUTPUT
class tImpulseSensorTypes
{
public:
	   typedef struct
	   {
	      uint16_t Count; // numer of impulses in last period
	      uint16_t Sum;   // sum of all impulses
	   } tResult_api_v1;

	   typedef struct
	   {
	      uint8_t Pin;
	   } tConfig_api_v1;
};
#endif


#if CONFIG_IMPULSE_SENSOR

class tImpulseSensor final : public tSensor, public tImpulseSensorTypes {
public:
   static const uint8_t API_VERSION = 1;
   typedef tConfig_api_v1 tConfig;
   typedef tResult_api_v1 tResult;


   tImpulseSensor(uint8_t sensorID);
   virtual ~tImpulseSensor() {}

   void CleanSum(); 			// clear Sum
   static void Impulse0(); // COUNTER TRIGGER - interrupt safe, may be called in interrupt handler
   static void Impulse1(); // COUNTER TRIGGER - interrupt safe, may be called in interrupt handler
   static void Impulse2(); // COUNTER TRIGGER - interrupt safe, may be called in interrupt handler

protected:
   virtual void doTriggerMeasurement();
   virtual uint8_t onSetConfig();
private:
   static const uint8_t IMPULSE_SENSOR_MAX_SENSORS = 3;

   tConfig Config;
   static volatile uint16_t mCnt[IMPULSE_SENSOR_MAX_SENSORS];
   uint8_t mSensorNum;
   static uint8_t mLastSensorNum;
   tResult mResult;
};

class tImpulseSensorLogger : public tSensorLogger
{
public:
    tImpulseSensorLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_IMPULSE, sensorID) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob);
};

#endif //CONFIG_IMPULSE_SENSOR
