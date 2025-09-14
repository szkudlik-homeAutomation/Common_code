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
	tSensorJsonFormatter() {};
	static tSensorJsonFormatter* Factory(uint8_t SensorType, uint8_t apiVersion);

	virtual uint8_t FormatJSON(Stream *pStream, tSensorCache *cache) = 0;
	virtual const char *getSensorTypeName();
};

#endif CONFIG_SENSORS_JSON_OUTPUT
