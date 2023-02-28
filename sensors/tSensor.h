/*
 * tSensor.h
 *
 * a generic class for all sensors in the system
 */

#pragma once


#include "../../../global.h"
#if CONFIG_SENSORS

#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"

#define SENSOR_PROCESS_SERVICE_TIME 100

#define SENSOR_TYPE_DS1820 1
#define SENSOR_TYPE_IMPULSE 2
#define SENSOR_TYPE_PT100_ANALOG 3
#define SENSOR_TYPE_DIGITAL_INPUT 4
#define SENSOR_TYPE_OUTPUT_STATES 5
#define SENSOR_TYPE_HEATING_CIRCLE_STATE 6
#define SENSOR_TYPE_SYSTEM_STATUS 7

#define SENSOR_ID_NOT_FOUND 0xff

typedef enum
{
   EV_TYPE_MEASUREMENT_ERROR,
   EV_TYPE_MEASUREMENT_COMPLETED,
   EV_TYPE_MEASUREMENT_CHANGE,
   EV_TYPE_THOLD_EXCEEDED
} tSensorEventType;

class tSensor;

class tSensorProcess : public Process
{
public:
   tSensorProcess(Scheduler &manager) :
    Process(manager,MEDIUM_PRIORITY,SENSOR_PROCESS_SERVICE_TIME) {}

   virtual void setup();
   virtual void service();

};


extern tSensorProcess SensorProcess;

#if CONFIG_SENSORS_JSON_OUTPUT
/* translate a data blob with sensor type to JSON data */
uint8_t TranslateBlobToJSON(uint8_t SensorType, uint8_t dataBlobSize, void *pDataCache, Stream *pStream);

/* an same, but for app specific sensors, implemented as weak stub */
uint8_t appTranslateBlobToJSON(uint8_t SensorType, uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
#endif //CONFIG_SENSORS_JSON_OUTPUT

class tSensor {
public:
   /*
    * @brief register a sensor to a local system with a given ID
    * check if sensor ID is duplicated on LOCAL SYSTEM ONLY
    * in case duplication on central node the sensor simply won't be registerd
    * send a register message to central system (if neccessary)
    *
    * @retval STATUS_SUCCESS
    * @retval STATUS_DUPLICATE_ID
    */
   uint8_t Register(uint8_t sensorID, char * pSensorName);
   uint8_t Register(uint8_t sensorID, char * pSensorName, void *pConfigBlob, uint16_t measurementPeriod)
   {
      SetMeasurementPeriod(measurementPeriod);
      if (pConfigBlob)
         SetSpecificConfig(pConfigBlob);
      uint8_t result = Register(sensorID,pSensorName);
      return result;
   }

   void SetMeasurementPeriod(uint16_t period)   // time in number of calls to Run() A tick = SENSOR_PROCESS_SERVICE_TIME
   {
      mMeasurementPeriod = period;
      mCurrMeasurementPeriod = period;
   }

   uint16_t GetMeasurementPeriod() const { return mMeasurementPeriod; }

   virtual uint8_t SetSpecificConfig(void *pBlob) {return STATUS_SUCCESS;}


   void TriggerMeasurement() { if (isRunning()) doTriggerMeasurement(); }

   bool isRunning() const { return mConfigSet; } // to be extended

   static void Run();

   uint8_t getSensorType() const { return mSensorType; }
   const bool isMeasurementValid() { return misMeasurementValid; }   // false if not triggered or measurement error

   uint8_t getSensorID() const { return mSensorID; }
   static tSensor* getSensor(uint8_t sensorID);

   /* the sensor may be located on a remote node, controler has only data blobs and need to translate it to JSON
    * Sensors should put a simple string to pStream
    * "MetricName": metric_value, "MetricName1": metric_value1,
    *
    * no braces etc.
    * there MUST be a comma at the last character, unless there's no data at all
    */
protected:
   tSensor(uint8_t SensorType);


   bool mConfigSet;
   void *mCurrentMeasurementBlob;
   uint8_t mMeasurementBlobSize;

   void onMeasurementCompleted(bool Status);
   virtual void doTriggerMeasurement() = 0;
   virtual void doTimeTick() {};

private:
   static tSensor* pFirst;
   tSensor* pNext;

   uint8_t mSensorID;

   uint8_t mSensorType;

   uint16_t mMeasurementPeriod;
   uint16_t mCurrMeasurementPeriod;
   bool misMeasurementValid;
};
#endif // CONFIG_SENSORS
