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

   typedef enum
   {
      DS1820,
   } tSensorType;

   typedef void(*tSensorCallback)(tSensor *pSensor, tEventType EventType);

   tSensor(tSensorType SensorType)
         : mCurrentMeasurementBlob(NULL),
           mMeasurementBlobSize(0),
           mSensorType(SensorType),
           mConfigSet(false),
           mMeasurementPeriod(0),
           mCurrMeasurementPeriod(0),
           misMeasurementValid(false),
           mSensorCallback(NULL)

            { pNext = pFirst ; pFirst = this; }

   void SetMeasurementPeriod(uint16_t period)   // time in number of calls to Run() A tick = SENSOR_PROCESS_SERVICE_TIME
   {
      mMeasurementPeriod = period;
      mCurrMeasurementPeriod = period;
   }

   void SetCalback(tSensorCallback SensorCallback) { mSensorCallback = SensorCallback; }

   virtual void SetSpecificConfig(void *pBlob) {};
   void TriggerMeasurement() { if (mConfigSet) doTriggerMeasurement(); }

   static void Run();

   tSensorType getSensorType() const { return mSensorType; }
   const bool isMeasurementValid() { return misMeasurementValid; }   // false if not triggered or measurement error
   const void* getMeasurementBlob() const { return mCurrentMeasurementBlob; }
   uint8_t getMeasurementBlobSize() const { return mMeasurementBlobSize; }

protected:
   bool mConfigSet;
   void *mCurrentMeasurementBlob;
   uint8_t mMeasurementBlobSize;

   void onMeasurementCompleted(bool Status);
   virtual void doTriggerMeasurement() = 0;
   virtual void doTimeTick() {};

private:
   static tSensor* pFirst;
   tSensor* pNext;

   tSensorType mSensorType;

   uint16_t mMeasurementPeriod;
   uint16_t mCurrMeasurementPeriod;
   bool misMeasurementValid;

   tSensorCallback mSensorCallback;
};

tSensor *SensorFactory(uint8_t SensorType);

#endif /* SRC_TSENSOR_H_ */
