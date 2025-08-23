#pragma once

#include "../../../global.h"

#if CONFIG_SENSORS_JSON_OUTPUT

class tSensorJsonFormatter;
class tSensorCache;


class tSensorJsonFormatterFactory
{
public:
	static tSensorJsonFormatterFactory* Instance;
	tSensorJsonFormatterFactory() { Instance = this; };
	virtual ~tSensorJsonFormatterFactory() { Instance = NULL; }

	tSensorJsonFormatter *createJsonFormatter(uint8_t SensorType, uint8_t apiVersion);

protected:
	   /**
	   * Application callback for app specific sensors factory, called when sensor has not been found by SensorCacheFactory
	   */
	  virtual tSensorJsonFormatter *appSpecificCreateJsonFormatter(uint8_t SensorType, uint8_t apiVersion) { return NULL; }
};

class tSensorJsonFormatter
{
public:
	tSensorJsonFormatter() : mHidden(false) {};
	static tSensorJsonFormatter* Factory(uint8_t SensorType, uint8_t apiVersion);

	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) = 0;

	//get JSON output in format for aggregated sensors
#if CONFIG_SENSOR_HUB_AGGREGATE
	virtual uint8_t FormatAggreatedPrefix(Stream *pStream) { return STATUS_UNSUPPORTED; };
	virtual uint8_t FormatAggreatedJSON(Stream *pStream, tSensorCache *cache) { return STATUS_UNSUPPORTED; };
	virtual uint8_t FormatAggreatedSuffix(Stream *pStream) { return STATUS_UNSUPPORTED; };
#endif //CONFIG_SENSOR_HUB_AGGREGATE
	bool isHidden() { return mHidden; }
protected:
	bool mHidden;
};

#endif CONFIG_SENSORS_JSON_OUTPUT
