#pragma once

#include "../../../global.h"

#if CONFIG_SENSORS_JSON_OUTPUT

class tSensorCache;
typedef uint8_t (*doFormatJSON)(Stream *pStream, tSensorCache *cache);

class tSensorJsonOutput
{
public:
	static tSensorJsonOutput* Instance;

	tSensorJsonOutput() { Instance = this; };
	virtual ~tSensorJsonOutput() { Instance = NULL; }

   /* get a function pointer to JSON encoder for given sensor type and API version. Null if no matching function found */
   doFormatJSON getJSONFormatFunction(uint8_t SensorType, uint8_t apiVersion);

protected:
   /**
   * Application callback for app specific sensors factory, called when sensor has not been found by SensorCacheFactory
   */
  virtual doFormatJSON  appSpecificGetSJONFrormatFunction(uint8_t SensorType, uint8_t apiVersion) { return NULL; }

};

#endif CONFIG_SENSORS_JSON_OUTPUT
