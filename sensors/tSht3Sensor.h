
#pragma once

#include "../../../global.h"

#if CONFIG_SHT3_DIS_SENSOR

#include "tSensor.h"
#include "tSensorLogger.h"
#include "../../lib/sht3x/src/SensirionI2cSht3x.h"

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t Sht3SensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache);
#endif //CONFIG_SENSORS_JSON_OUTPUT


class tSht3Sensor: public tSensor {
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

class tSht3SensorLogger : public tSensorLogger
{
public:
	tSht3SensorLogger(uint8_t sensorID) : tSensorLogger(SENSOR_TYPE_SHT3, sensorID) {}
	tSht3SensorLogger() : tSensorLogger(SENSOR_TYPE_SHT3, 0) {}
protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob);
};

#endif CONFIG_SHT3_DIS_SENSOR
