/*
 * tSensor.h
 *
 * a generic class for all sensors in the system
 */

#pragma once


#include "../../../global.h"
#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"

#define SENSOR_PROCESS_SERVICE_TIME 100

#define SENSOR_TYPE_DS1820 1
#define SENSOR_TYPE_IMPULSE 2
#define SENSOR_TYPE_PT100_ANALOG 3
#define SENSOR_TYPE_DIGITAL_INPUT 4
#define SENSOR_TYPE_OUTPUT_STATES 5
#define SENSOR_TYPE_HEATING_CIRCLE_STATE 6

#define SENSOR_ID_NOT_FOUND 0xff

typedef enum
{
   EV_TYPE_MEASUREMENT_ERROR,
   EV_TYPE_MEASUREMENT_COMPLETED,
   EV_TYPE_MEASUREMENT_CHANGE,
   EV_TYPE_THOLD_EXCEEDED
} tSensorEventType;

class tSensor;
class tSensorEvent	//TODO - events by sensorHub only
{
public:
   tSensorEvent() : mpNext(NULL) {}
   virtual ~tSensorEvent() {}

   virtual void onEvent(tSensor *pSensor, tSensorEventType EventType) = 0;
private:
   tSensorEvent *mpNext; friend class tSensor;
};

class tSensorProcess : public Process
{
public:
   tSensorProcess(Scheduler &manager) :
    Process(manager,MEDIUM_PRIORITY,SENSOR_PROCESS_SERVICE_TIME) {}

   virtual void setup();
   virtual void service();

};


extern tSensorProcess SensorProcess;

class tSensor {
public:
   /*
    * @brief create a sensor on local system
    * sensor poiner is avaliable using getsensor
    *
    */
   static uint8_t Create(uint8_t SensorType, uint8_t sensorID);

   void SetMeasurementPeriod(uint16_t period)   // time in number of calls to Run() A tick = SENSOR_PROCESS_SERVICE_TIME
   {
      mMeasurementPeriod = period;
      mCurrMeasurementPeriod = period;
   }

   uint16_t GetMeasurementPeriod() const { return mMeasurementPeriod; }

   void SetEventCalback(tSensorEvent *pEvent)
   {
      pEvent->mpNext = mpFirstEvent;
      mpFirstEvent = pEvent;
   }

   virtual uint8_t SetSpecificConfig(void *pBlob) {return STATUS_SUCCESS;}


   void TriggerMeasurement() { if (isRunning()) doTriggerMeasurement(); }

   bool isRunning() const { return mConfigSet; } // to be extended

   static void Run();

   uint8_t getSensorType() const { return mSensorType; }
   const bool isMeasurementValid() { return misMeasurementValid; }   // false if not triggered or measurement error
   const void* getMeasurementBlob() const { return mCurrentMeasurementBlob; }
   uint8_t getMeasurementBlobSize() const { return mMeasurementBlobSize; }

   uint8_t getSensorID() const { return mSensorID; }
   static tSensor* getSensor(uint8_t sensorID);

   /* the sensor may be located on a remote node, controler has only data blobs and need to translate it to JSON
    * Sensors should put a simple string to pStream
    * "MetricName": metric_value, "MetricName1": metric_value1,
    *
    * no braces etc.
    * there MUST be a comma at the last character, unless there's no data at all
    */
   static uint8_t TranslateBlobToJSON(uint8_t SensorType, uint8_t dataBlobSize, void *pDataCache, Stream *pStream);
   /* shortcut to get json from local sensors */
   uint8_t GetJSON(uint8_t SensorType, Stream *pStream)
   {
      return TranslateBlobToJSON(SensorType, getMeasurementBlobSize(), getMeasurementBlob(), pStream);
   }

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

   tSensorEvent *mpFirstEvent;
};
