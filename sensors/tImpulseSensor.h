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
#include "tSensorJsonOutput.h"

#if CONFIG_IMPULSE_SENSOR_JSON_OUTPUT
class tSensorJsonFormatter_ImpulseSensor_api_1 : public tSensorJsonFormatter
{
public:
	tSensorJsonFormatter_ImpulseSensor_api_1() : tSensorJsonFormatter() {}
protected:
	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) override;
};
#endif //CONFIG_IMPULSE_SENSOR_JSON_OUTPUT

#if CONFIG_IMPULSE_SENSOR || CONFIG_IMPULSE_SENSOR_JSON_OUTPUT || CONFIG_SENSOR_LOGGER
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

#endif //CONFIG_IMPULSE_SENSOR

#if CONFIG_SENSOR_LOGGER

class tImpulseSensorLogger : public tSensorLogger
{
public:
    tImpulseSensorLogger() : tSensorLogger(SENSOR_TYPE_IMPULSE, 0) {}
    tImpulseSensorLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_IMPULSE, sensorID) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob) override;
};
#endif CONFIG_SENSOR_LOGGER

