/*
 * tSht3Sensor.cpp
 *
 *  Created on: 7 kwi 2025
 *      Author: szkud
 */

//#define LOCAL_LOGLEVEL_2 1

#include "../../../global.h"


#include "tSht3Sensor.h"

#if CONFIG_SHT3_DIS_SENSOR_JSON_OUTPUT

uint8_t tSensorJsonFormatter_Sht3Sensor_api_1::FormatJSON(Stream *pStream, tSensorCache *cache)
{
   if (cache->getDataBlobSize() != sizeof(tSht3SensorTypes::tResult_api_v1))
   {
		 return STATUS_JSON_ENCODE_ERROR;
   }

   tSht3SensorTypes::tResult_api_v1 *pResult =(tSht3SensorTypes::tResult_api_v1 *) cache->getData();
   pStream->print(F("\"Temperature\":"));
   pStream->print(pResult->Temperature);
   pStream->print(F(", \"Humidity\":"));
   pStream->print(pResult->Humidity);

   return STATUS_SUCCESS;
}

const char *tSensorJsonFormatter_Sht3Sensor_api_1::getSensorTypeName()
{
	static const char IdPrefix[] PROGMEM = "SHT3";
	return IdPrefix;
}

#endif // CONFIG_SHT3_DIS_SENSOR_JSON_OUTPUT

#if CONFIG_SHT3_DIS_SENSOR
#include <Wire.h>

uint8_t tSht3Sensor::onSetConfig()
{
    Wire.begin();
    sensor.begin(Wire, Config.I2C_Addr);

	ClearShadowResult();
	tickCnt = 0;
    sensor.stopMeasurement();
    delay(1);
    sensor.softReset();

    delay(100);
    uint16_t aStatusRegister = 0u;
    int16_t error = sensor.readStatusRegister(aStatusRegister);
    if (error != NO_ERROR) {
    	DEBUG_PRINTLN_3("tSht3Sensor: Error reading status register");
        return STATUS_GENERAL_FAILURE;
    }

    error = sensor.startPeriodicMeasurement(REPEATABILITY_MEDIUM,
                                            MPS_ONE_PER_SECOND);
    if (error != NO_ERROR) {
    	DEBUG_PRINTLN_3("tSht3Sensor: Error setting periodic measurement");
        return STATUS_GENERAL_FAILURE;
    }

    return STATUS_SUCCESS;
}

void tSht3Sensor::doTimeTick()
{
	tickCnt++;
	if (tickCnt < CONFIG_SHT3_DIS_SENSOR_MEASURE_PERIOD * (1000 / SENSOR_PROCESS_SERVICE_TIME))
		return;

	// do measurement

	float temperature;
	float humidity;
	uint8_t error = sensor.blockingReadMeasurement(temperature, humidity);	// blocking is fine, measurement takes 1sec, default period is 2sec.
	if (error != NO_ERROR) {
    	DEBUG_PRINTLN_3("tSht3Sensor: Error in measurement");
    	return;
	}
	if (Config.Avg)
	{
		mShadowResult.Humidity += humidity;
		mShadowResult.Temperature += temperature;
		avgCnt++;
	}
	else
	{
		mShadowResult.Humidity = humidity;
		mShadowResult.Temperature = temperature;
	}

	DEBUG_PRINT_2("tSht3Sensor tick: temperature: ");
	DEBUG_2(print(temperature));
	DEBUG_PRINT_2(" humidity: ");
	DEBUG_2(println(humidity));

	tickCnt = 0;
}

void tSht3Sensor::doTriggerMeasurement()
{
	if (Config.Avg && avgCnt)
	{
		mResult.Humidity = mShadowResult.Humidity / avgCnt;
		mResult.Temperature = mShadowResult.Temperature / avgCnt;
	}
	else
	{
		mResult.Humidity = mShadowResult.Humidity;
		mResult.Temperature = mShadowResult.Temperature;
	}
	ClearShadowResult();

	DEBUG_PRINT_2("tSht3Sensor: temperature: ");
	DEBUG_2(print(mResult.Temperature));
	DEBUG_PRINT_2(" humidity: ");
	DEBUG_2(println(mResult.Humidity));

	onMeasurementCompleted(true);
}

#endif CONFIG_SHT3_DIS_SENSOR


#if CONFIG_SENSOR_LOGGER
void tSht3SensorLogger::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob)
{
    if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
        //TODO
        return;

    pResult = (tSht3SensorTypes::tResult_api_v1 *)pDataBlob;

    LOG_PRINT("tSht3Sensor Measurement completed. SensorID: ");
	LOG(print(pResult->Temperature));
	LOG_PRINT(" humidity: ");
	LOG(println(pResult->Humidity));
}
#endif // CONFIG_SENSOR_LOGGER

