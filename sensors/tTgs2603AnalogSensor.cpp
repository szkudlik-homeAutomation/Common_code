/*
 * tPt100AnalogSensor.cpp
 *
 *  Created on: 21 pa� 2022
 *      Author: szkud
 */

//#define LOCAL_LOGLEVEL_2 1

#include "../../../global.h"
#if CONFIG_TGS2603_ODOUR_SENSOR

#include "tTgs2603AnalogSensor.h"


//#if CONFIG_SENSORS_JSON_OUTPUT
//uint8_t Pt100AnalogSensorJsonFormat_api_1(Stream *pStream, tSensorCache *cache){
//   if (cache->getDataBlobSize() != sizeof(tPt100AnalogSensor::tResult))
//   {
//         return STATUS_JSON_ENCODE_ERROR;
//   }
//
//   tPt100AnalogSensor::tResult *pResult =(tPt100AnalogSensor::tResult *) cache->getData();
//   pStream->print(F("\"Temperature\":"));
//   pStream->print(pResult->Temperature);
//   return STATUS_SUCCESS;
//}
//#endif CONFIG_SENSORS_JSON_OUTPUT

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

void tTgs2603AnalogLogger::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
{
    if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
        //TODO
        return;

    tTgs2603AnalogSensor::tResult *pResult = (tTgs2603AnalogSensor::tResult *)pDataBlob;

    LOG_PRINT("Tgs2603 Measurement completed. SensorID: ");
    LOG(print(SensorID));
    LOG_PRINT(" Odour: ");
    LOG(println(pResult->Odour));
}

#endif //CONFIG_TGS2603_ODOUR_SENSOR
