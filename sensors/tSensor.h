/*
 * tSensor.h
 *
 * a generic class for all sensors in the system
 */

#ifndef SRC_TSENSOR_H_
#define SRC_TSENSOR_H_

#include "../../../global.h"
#include <ProcessScheduler.h>

#define SENSOR_PROCESS_SERVICE_TIME 100

// sensor types - equal to communication sensor types
#define SENSOR_TYPE_DS1820 1
//MESSAGE_TYPE_CREATE_SENSOR_TYPE_DS1820

// error codes - equal to communication error codes
#define CREATE_SENSOR_STATUS_OK 0
//MESSAGE_TYPE_CREATE_SENSOR_STATUS_SUCCESS
#define CREATE_SENSOR_STATUS_DUPLICATE_ID 1
//MESSAGE_TYPE_CREATE_SENSOR_STATUS_DUPLICATE_ID
#define CREATE_SENSOR_STATUS_UNKNOWN_SENSOR 2
// MESSAGE_TYPE_CREATE_SENSOR_STATUS_UNKNOWN_SENSOR
#define CREATE_SENSOR_STATUS_OTHER_ERROR 3
//MESSAGE_TYPE_CREATE_SENSOR_STATUS_OTHER_ERROR


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
   typedef enum
   {
      EV_TYPE_MEASUREMENT_ERROR,
      EV_TYPE_MEASUREMENT_COMPLETED,
      EV_TYPE_MEASUREMENT_CHANGE,
      EV_TYPE_THOLD_EXCEEDED
   } tEventType;

   typedef void(*tSensorCallback)(tSensor *pSensor, tEventType EventType);

   // create a sensor. Return codes CREATE_SENSOR_STATUS*
   // sensor pointer avaliable through getSensor
   static uint8_t Create(uint8_t SensorType, uint8_t sensorID);

   void SetMeasurementPeriod(uint16_t period)   // time in number of calls to Run() A tick = SENSOR_PROCESS_SERVICE_TIME
   {
      mMeasurementPeriod = period;
      mCurrMeasurementPeriod = period;
   }

   uint16_t GetMeasurementPeriod() const { return mMeasurementPeriod; }

   void SetCalback(tSensorCallback SensorCallback) { mSensorCallback = SensorCallback; }

   virtual void SetSpecificConfig(void *pBlob) {};
   void TriggerMeasurement() { if (isRunning()) doTriggerMeasurement(); }

   bool isRunning() const { return mConfigSet; } // to be extended

   static void Run();

   uint8_t getSensorType() const { return mSensorType; }
   const bool isMeasurementValid() { return misMeasurementValid; }   // false if not triggered or measurement error
   const void* getMeasurementBlob() const { return mCurrentMeasurementBlob; }
   uint8_t getMeasurementBlobSize() const { return mMeasurementBlobSize; }

   uint8_t getSensorID() const { return mSensorID; }
   static tSensor* getSensor(uint8_t sensorID);

protected:
   tSensor(uint8_t SensorType, uint8_t sensorID);


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

   tSensorCallback mSensorCallback;
};

tSensor *SensorFactory(uint8_t SensorType);

#endif /* SRC_TSENSOR_H_ */
