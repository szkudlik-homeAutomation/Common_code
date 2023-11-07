/*
 * tSensorHub.cpp
 *
 *  Created on: Oct 28, 2022
 *      Author: mszkudli
 */

#include "../../../global.h"
#if CONFIG_SENSOR_HUB

#include "tSensorHub.h"
#include "tSensor.h"
#include "tSensorDesc.h"

static tSensorHub *tSensorHub::Instance = NULL;

uint8_t tSensorHub::getSensorID(const char * pSensorName)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByName(pSensorName);
   if (NULL == pSensorDesc)
   {
      return SENSOR_ID_NOT_FOUND;
   }

   return pSensorDesc->SensorID;
}

const char *tSensorHub::getSensorName(uint8_t SensorID)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return NULL;
   }
   return (pSensorDesc->pName);
}

uint8_t tSensorHub::getCachedSensorData(uint8_t SensorID,  uint8_t *dataBlobSize, void **pDataBlob)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
   }

   *dataBlobSize = pSensorDesc->dataBlobSize;
   *pDataBlob = pSensorDesc->pDataCache;
}


uint8_t tSensorHub::RegisterLocalSensor(uint8_t SensorID, char * pSensorName, uint8_t api_version)
{
	   DEBUG_PRINTLN_3("");
	   DEBUG_PRINT_3("==>Sensor register request. ID: ");
	   DEBUG_3(print(SensorID));
	   DEBUG_PRINT_3(" name: ");
	   DEBUG_3(println(pSensorName));

	   tSensor *pSensor = tSensor::getSensor(SensorID);
	   if (pSensor == NULL)
	   {
	      DEBUG_PRINTLN_3("-----> tSensor::getSensor error ");
	      return STATUS_UNKNOWN_SENSOR_ID;
	   }

	   // add sensor to repository
	   tSensorDesc *pSensorDesc = sensorDescFactory(
			   pSensor->getSensorType(),
			   SensorID,
			   pSensorName,
			   api_version);

	   // send response
	   DEBUG_PRINTLN_3("-----> DONE, SUCCESS");
	   return STATUS_SUCCESS;
}

uint8_t tSensorHub::subscribeToEvents(uint8_t SensorID, tSensorHubEvent *pSensorEvent)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
   }

   pSensorEvent->Connect(&pSensorDesc->pFirstEventHander);
   return STATUS_SUCCESS;
}

#if CONFIG_SENSORS_JSON_OUTPUT
uint8_t tSensorHub::getCachedSensorDataJson(uint8_t SensorID, Stream *pStream)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return STATUS_UNKNOWN_SENSOR_ID;
   }

   return pSensorDesc->formatJSON(pStream);
}

uint8_t tSensorHub::getCachedSensorsDataJson(Stream *pStream)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getFirst();
   while(NULL != pSensorDesc)
   {
      pSensorDesc->formatJSON(pStream);
      pSensorDesc = pSensorDesc->getNext();
      if (NULL!=pSensorDesc)
      {
         pStream->print(",");
      }
      pStream->print("\r\n");
   }
}

#endif // CONFIG_SENSORS_JSON_OUTPUT

void tSensorHub::callAllCallbacks(tSensorDesc *pSensorDesc, tSensorEventType EventType)
{
   // callbacks
   tSensorHubEvent *pEventCallback = pSensorDesc->pFirstEventHander;
   while (pEventCallback)
   {
      if (EV_TYPE_MEASUREMENT_ERROR != EventType)
         pEventCallback->onEvent(pSensorDesc->SensorID, EventType, pSensorDesc->dataBlobSize, pSensorDesc->pDataCache);
      else
         pEventCallback->onEvent(pSensorDesc->SensorID, EventType, 0, NULL);

      pEventCallback = pEventCallback->pNext;
   }
}

void tSensorHub::onSensorEvent(uint8_t SensorID, tSensorEventType EventType, uint8_t dataBlobSize, void *pDataBlob)
{
   tSensorDesc *pSensorDesc = tSensorDesc::getByID(SensorID);
   if (NULL == pSensorDesc)
   {
      return;
   }

   if (EventType == EV_TYPE_MEASUREMENT_ERROR)
   {
      pSensorDesc->Status = STATUS_SENSOR_ERROR_REPORTED;
      // callbacks
      callAllCallbacks(pSensorDesc,EV_TYPE_MEASUREMENT_ERROR);
      return;
   }

   if ((pSensorDesc->dataBlobSize == 0) && (dataBlobSize > 0))
   {
      pSensorDesc->dataBlobSize = dataBlobSize;
      pSensorDesc->pDataCache = malloc(dataBlobSize);
   }

   if (pSensorDesc->dataBlobSize != dataBlobSize)
   {
      pSensorDesc->Status = STATUS_INCORRECT_DATA_SIZE;
      callAllCallbacks(pSensorDesc,EV_TYPE_MEASUREMENT_ERROR);
      return;
   }

   pSensorDesc->Status = STATUS_SUCCESS;
   memcpy(pSensorDesc->pDataCache,pDataBlob,dataBlobSize);

   // callbacks
   callAllCallbacks(pSensorDesc,EventType);
}

#if CONFIG_DS1820_SENSOR
#include "tDS1820Sensor.h"
#endif

#if CONFIG_IMPULSE_SENSOR
#include "tImpulseSensor.h"
#endif

#if CONFIG_PT100_ANALOG_SENSOR
#include "tPt100AnalogSensor.h"
#endif

#if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
#include "tSimpleDigitalInputSensor.h"
#endif

#if CONFIG_OUTPUT_STATE_SENSOR
#include "tOutputStateSensor.h"
#endif

#if CONFIG_SYSTEM_STATUS_SENSOR
#include "tSystemStatusSensor.h"
#endif


tSensorDesc *tSensorHub::sensorDescFactory(uint8_t SensorType, uint8_t SensorID, char * pSensorName, uint8_t apiVersion)
{
    tSensorDesc *newSensorDesc = NULL;
    switch (SensorType)
    {
    #if CONFIG_DS1820_SENSOR
          case SENSOR_TYPE_DS1820:
              newSensorDesc = new tDs1820SensorDesc();
              break;
    #endif // CONFIG_DS1820_SENSOR

    #if CONFIG_IMPULSE_SENSOR
          case SENSOR_TYPE_IMPULSE:
             newSensorDesc = new tImpulseSensorDesc();
             break;
    #endif //CONFIG_IMPULSE_SENSOR

    #if CONFIG_PT100_ANALOG_SENSOR
             case SENSOR_TYPE_PT100_ANALOG:
                 newSensorDesc = new tPt100AnalogSensorDesc();
                 break;
    #endif

    #if CONFIG_SIMPLE_DIGITAL_INPUT_SENSOR
          case SENSOR_TYPE_DIGITAL_INPUT:
             newSensorDesc = new tSimpleDigitalInputSensorDesc();
             break;
    #endif

    #if CONFIG_OUTPUT_STATE_SENSOR
         case SENSOR_TYPE_OUTPUT_STATES:
             newSensorDesc = new tOutputStateSensorDesc();
             break;
    #endif

    #if CONFIG_SYSTEM_STATUS_SENSOR
          case SENSOR_TYPE_SYSTEM_STATUS:
              newSensorDesc = new tSystemStatusSensorDesc();
              break;
    #endif //CONFIG_SYSTEM_STATUS_SENSOR

          default:
              newSensorDesc = appSpecificSenorDescFactory(SensorType);
    }

    if (NULL == newSensorDesc)
        newSensorDesc = new tSensorDesc(); /* generic, no JSON output */

    newSensorDesc->sensorApiVersion = apiVersion;
    newSensorDesc->sensorType = SensorType;
    newSensorDesc->SensorID = SensorID;
    newSensorDesc->pName = pSensorName;

    return newSensorDesc;
}


#endif //CONFIG_SENSOR_HUB
