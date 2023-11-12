/*
 * tSensor.h
 *
 * a generic class for all sensors in the system
 */

#pragma once

/*
 * Sensor creation sequence
 * 1) create an object - dyn or static
 * 2) fill the config - specific to the sensor
 * 		2a) generic "getConfigBlob" may be used in case of dynamic creation of sensors i.e. from eeprom
 * 3) call "setConfig" with measurementPeriod
 * 4) optional: call "register" with sensor ID and name
 * 5) call "Start"
 *
 * register may be called multiple times, especially in case of remote sensors
 *
 * the sensor should be running at this point
 */

#include "../../../global.h"
#if CONFIG_SENSORS

#include "../../lib/ArduinoProcessScheduler/src/ProcessScheduler.h"

/* sensor time tick - 100ms */
#define SENSOR_PROCESS_SERVICE_TIME 100

#define SENSOR_TYPE_DS1820 1
#define SENSOR_TYPE_IMPULSE 2
#define SENSOR_TYPE_PT100_ANALOG 3
#define SENSOR_TYPE_DIGITAL_INPUT 4
#define SENSOR_TYPE_OUTPUT_STATES 5
#define SENSOR_TYPE_HEATING_CIRCLE_STATE 6
#define SENSOR_TYPE_SYSTEM_STATUS 7

#define SENSOR_ID_NOT_FOUND 0xff

#define EV_TYPE_MEASUREMENT_COMPLETED BIT0
#define EV_TYPE_MEASUREMENT_ERROR  BIT1
#define EV_TYPE_MEASUREMENT_CHANGE BIT2
#define EV_TYPE_THOLD_EXCEEDED BIT3

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

class tSensor {
public:
	/* process and set config. The config must be set before either by sensor's specific functions
	 * setConfig MUST be called once
	 *
	 * if pConfigBlob is provided, it must point to blob of config of size equal to mConfigBlobSize
	 * the config will be copied to specific sensor config
	 */
	uint8_t setConfig(uint8_t SensorID, uint16_t measurementPeriod, void *pConfigBlob = NULL);

	void setSensorSerialEventsMask(uint8_t mask) { mSerialEventsMask = mask; }
	uint8_t getSensorSerialEventsMask() const { return mSerialEventsMask; }

	/* make the sensor running */
	uint8_t Start()
	{
		if (SENSOR_PAUSED == mState)
		{
			mState = SENSOR_RUNNING;
			return doRun();
		}
		return STATUS_SENSOR_INCORRECT_STATE;
	}

	/* pause the sensor */
	uint8_t Pause()
	{
		if (SENSOR_RUNNING == mState)
		{
			mState = SENSOR_PAUSED;
			return doPause();
		}
		return STATUS_SENSOR_INCORRECT_STATE;
	}

   /* register the sensor in sensor hub.
    * either local or remote in case of remote nodes
    */
   uint8_t Register(char * pSensorName);

   uint16_t GetMeasurementPeriod() const { return mMeasurementPeriod; }
   void TriggerMeasurement() { if (isRunning()) doTriggerMeasurement(); }
   bool isRunning() const { return (mState == SENSOR_RUNNING); }
   bool isConfigured() const { return (mState > SENSOR_CREATED); }
   uint8_t getSensorType() const { return mSensorType; }
   const bool isMeasurementValid() { return misMeasurementValid; }   // false if not triggered or measurement error
   uint8_t getSensorID() const { return mSensorID; }
   uint8_t getSensorApiVersion() const { return mApiVersion; }
   uint8_t getConfigBlobSize() const { return mConfigBlobSize; }

   static tSensor* getSensor(uint8_t sensorID);

   static void Run();

#if CONFIG_TLE8457_COMM_LIB
   void sendMsgSensorEvent(bool onDemand);
#endif //CONFIG_TLE8457_COMM_LIB
protected:
   /* ApiVersion - the sensor may be located on remote node, and its version may not match the central node
    * For identification, API version must be increased every time the config OR the result data format changes
    */
   tSensor(uint8_t SensorType, uint8_t ApiVersion, uint8_t ConfigBlobSize, void *ConfigBlobPtr);

   void *mCurrentMeasurementBlob;
   uint8_t mMeasurementBlobSize;

   void onMeasurementCompleted(bool Status);

   virtual void doTriggerMeasurement() = 0;
   virtual uint8_t doSetConfig() { return STATUS_SUCCESS; }
   virtual uint8_t doRun() { return STATUS_SUCCESS; }
   virtual uint8_t doPause() { return STATUS_SUCCESS; }
   virtual void doTimeTick() {};

private:
   static const uint8_t SENSOR_CREATED = 0;
   static const uint8_t SENSOR_PAUSED = 1;
   static const uint8_t SENSOR_RUNNING = 2;

   uint8_t mState;

   uint8_t mSensorID;

   uint8_t mSensorType;
   uint8_t mApiVersion;

   uint16_t mMeasurementPeriod;
   uint16_t mCurrMeasurementPeriod;
   bool misMeasurementValid;
   uint8_t mSerialEventsMask;
   uint8_t mConfigBlobSize;
   void *mConfigBlobPtr;

   static tSensor* pFirst;
   tSensor* pNext;
};
#endif // CONFIG_SENSORS
