/*
 * tPt100AnalogSensor.cpp
 *
 *  Created on: 21 pa� 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tTgs2603AnalogSensor.h"



#if CONFIG_TGS2603_ODOUR_SENSOR_JSON_OUTPUT

uint8_t tSensorJsonFormatter_Tgs2603OdourSensor_api_1::FormatJSON(Stream *pStream, tSensorCache *cache)
{
	   if (cache->getDataBlobSize() != sizeof(tTgs2603SensorTypes::tResult_api_v1))
	   {
			 return STATUS_JSON_ENCODE_ERROR;
	   }

	   tTgs2603SensorTypes::tResult_api_v1 *pResult =(tTgs2603SensorTypes::tResult_api_v1 *) cache->getData();
	   pStream->print(F("\"Odour\":"));
	   pStream->print(pResult->Odour);

	   return STATUS_SUCCESS;
}

const char *tSensorJsonFormatter_Tgs2603OdourSensor_api_1::getSensorTypeName()
{
	static const char IdPrefix[] PROGMEM = "TGS2603_ODOUR";
	return IdPrefix;
}

#endif // CONFIG_TGS2603_ODOUR_SENSOR_JSON_OUTPUT


#if CONFIG_TGS2603_ODOUR_SENSOR

void tTgs2603AnalogSensor::doTimeTick()
{
   int Read = analogRead(Config.Pin);
   OdourAvg = OdourAvg * 0.9 + Read * 0.1;

	DEBUG_PRINT_2("Tgs2603 Odour: ");
	DEBUG_2(println(Read));

}

void tTgs2603AnalogSensor::doTriggerMeasurement()
{
      mResult.Odour = round (OdourAvg);
      onMeasurementCompleted(true);
}


#endif // CONFIG_TGS2603_ODOUR_SENSOR



#if CONFIG_SENSOR_LOGGER
void tTgs2603AnalogLogger::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob)
{
    if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
        return;

    tTgs2603SensorTypes::tResult_api_v1 *pResult = (tTgs2603SensorTypes::tResult_api_v1 *)pDataBlob;

    LOG_PRINT("Tgs2603 SensorID: ");
    LOG(print(SensorID));
    LOG_PRINT(" Odour: ");
    LOG(println(pResult->Odour));
}

#endif //CONFIG_TGS2603_ODOUR_SENSOR
