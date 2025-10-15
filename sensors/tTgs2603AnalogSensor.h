#pragma once


#pragma once

#include "../../../global.h"
#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorLogger.h"
#include "tSensorJsonOutput.h"



#if CONFIG_TGS2603_ODOUR_SENSOR_JSON_OUTPUT

class tSensorJsonFormatter_Tgs2603OdourSensor_api_1 : public tSensorJsonFormatter
{
public:
	tSensorJsonFormatter_Tgs2603OdourSensor_api_1() : tSensorJsonFormatter() {}
protected:
	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) override;
	virtual const char *getSensorTypeName() override;
};

#endif //CONFIG_TGS2603_ODOUR_SENSOR_JSON_OUTPUT

#if CONFIG_SHT3_DIS_SENSOR || CONFIG_TGS2603_ODOUR_SENSOR_JSON_OUTPUT || CONFIG_SENSOR_LOGGER
class tTgs2603SensorTypes
{
public:
	typedef struct
	   {
	      uint8_t Pin;
	   } tConfig_api_v1;

	typedef struct
	   {
	      uint16_t Odour;
	   } tResult_api_v1;
};


#endif

#if CONFIG_TGS2603_ODOUR_SENSOR
class tTgs2603AnalogSensor : public tSensor, public tTgs2603SensorTypes {
public:

   static const uint8_t API_VERSION = 1;
   typedef tConfig_api_v1 tConfig;
   typedef tResult_api_v1 tResult;

   tConfig Config;

   tTgs2603AnalogSensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_TGS2603, sensorID, API_VERSION, sizeof(Config), &Config)
   {
	   mCurrentMeasurementBlob = (void*) &mResult;
	   mMeasurementBlobSize = sizeof(mResult);
	   OdourAvg = 0;
   }

protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();

private:
   tResult mResult;
   float OdourAvg;
};
#endif // CONFIG_TGS2603_ODOUR_SENSOR


#if CONFIG_SENSOR_LOGGER

class tTgs2603AnalogLogger : public tSensorLogger
{
public:
	tTgs2603AnalogLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_TGS2603, sensorID) {}
	tTgs2603AnalogLogger() : tSensorLogger(SENSOR_TYPE_TGS2603, 0) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob) override;
};

#endif // CONFIG_SENSOR_LOGGER

