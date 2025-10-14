
#pragma once

#include "../../../global.h"
#include "tSensor.h"
#include "tSensorCache.h"
#include "tSensorLogger.h"
#include "tSensorJsonOutput.h"

#if CONFIG_SHT3_DIS_SENSOR_JSON_OUTPUT

class tSensorJsonFormatter_Sht3Sensor_api_1 : public tSensorJsonFormatter
{
public:
	tSensorJsonFormatter_Sht3Sensor_api_1() : tSensorJsonFormatter() {}
protected:
	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) override;
	virtual const char *getSensorTypeName() override;
};
#endif //CONFIG_SHT3_DIS_SENSOR_JSON_OUTPUT

#if CONFIG_SHT3_DIS_SENSOR || CONFIG_SHT3_DIS_SENSOR_JSON_OUTPUT || CONFIG_SENSOR_LOGGER

class tSht3SensorTypes
{
public:
	typedef struct
	{
		float Temperature;
		float Humidity;
	} tResult_api_v1;

	typedef struct
	{
		uint8_t I2C_Addr;
		uint8_t Avg; 		// if false, the result will be the last measurement
							// if true, the result will be an avg from all measurements since last reading
	} tConfig_api_v1;
};

#endif

#if CONFIG_SHT3_DIS_SENSOR

#include "../../lib/sht3x/src/SensirionI2cSht3x.h"

class tSht3Sensor : public tSensor, public tSht3SensorTypes {
public:
	static const uint8_t API_VERSION = 1;
	typedef tResult_api_v1 tResult;
    typedef tConfig_api_v1 tConfig;

	tSht3Sensor(uint8_t sensorID) : tSensor(SENSOR_TYPE_SHT3, sensorID, API_VERSION,sizeof(Config), &Config)
	{
		   mCurrentMeasurementBlob = (void*) &mResult;
		   mMeasurementBlobSize = sizeof(mResult);
	}

protected:
   virtual void doTimeTick();
   virtual void doTriggerMeasurement();
   virtual uint8_t onSetConfig();

private:
   tResult mResult;
   tResult mShadowResult;
   uint8_t avgCnt;
   uint8_t tickCnt;
   tConfig Config;

   void ClearShadowResult()
   {
	   mShadowResult.Humidity = 0;
	   mShadowResult.Temperature = 0;
	   avgCnt = 0;
   }

   SensirionI2cSht3x sensor;
};
#endif CONFIG_SHT3_DIS_SENSOR

#if CONFIG_SENSOR_LOGGER
class tSht3SensorLogger : public tSensorLogger
{
public:
	tSht3SensorLogger() : tSensorLogger(SENSOR_TYPE_SHT3, 0) {}
	tSht3SensorLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_SHT3, sensorID) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob) override;
};
#endif


