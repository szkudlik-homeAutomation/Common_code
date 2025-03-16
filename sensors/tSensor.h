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
 * 5) call "Start"
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
#define SENSOR_TYPE_SYSTEM_STATUS 6
#define SENSOR_TYPE_WIEGAND 7

// app sensor types should start from 128

#define SENSOR_ID_NOT_FOUND 0xff

#define EV_TYPE_MEASUREMENT_COMPLETED 0
#define EV_TYPE_MEASUREMENT_ERROR  1
#define EV_TYPE_MEASUREMENT_CHANGE 2
#define EV_TYPE_THOLD_EXCEEDED 3
#define EV_TYPE_SENSOR_STATE_CHANGE 4


class tSensor;

class tSensorProcess : public Process
{
public:
    tSensorProcess() :
        Process(MEDIUM_PRIORITY,SENSOR_PROCESS_SERVICE_TIME)
    {
        Instance = this;
    }

   static tSensorProcess *Instance;

   virtual void setup();
   virtual void service();

};

class tSensor {
public:
#if CONFIG_SENSORS_STORE_IN_EEPROM
    /*
     * save all existing sensors to eeprom
     */
    static uint8_t SaveToEEprom();

    /* restore all sensors from eeprom
     * all sensor already existing sensors will be ignored
     */
    static uint8_t RestoreFromEEprom();

    /* remove all sensors from eeprom */
    static uint8_t DeleteAllSensorsFromEeprom();

#endif //CONFIG_SENSORS_STORE_IN_EEPROM

	/* process and set config. The config must be set before
	 *  - by sensor's specific functions
	 *  - or by setParitalConfig
	 * setConfig MUST be called once
	 *
	 * if pConfigBlob is provided, it must point to blob of config of size equal to mConfigBlobSize
	 * the config will be copied to sensor internal config
	 *
	 * ApiVersion will be checked if provided (!= 0)
	 *
	 * period in SENSOR_PROCESS_SERVICE_TIME unit
	 */
	uint8_t setConfig(uint16_t measurementPeriod)
	{
		return setConfig(measurementPeriod, 0, NULL, 0);
	}

	uint8_t setConfig(uint16_t measurementPeriod, uint8_t ApiVersion)
	{
		return setConfig(measurementPeriod, ApiVersion, NULL, 0);
	}
	uint8_t setConfig(uint16_t measurementPeriod, uint8_t ApiVersion, void *pConfigBlob, uint8_t configBlobSize);

	/* handle partial config
	 * seq numbers must be in order, any missing part will result in an error
	 *
	 * note!
	 * Due to limitation in serial data transfer, data are always comming in the const sizes chunks
	 * provided here as ChunkSize
	 * setParitalConfig must check size of config and copy the required part of data only
	 *
	 * if seq sequence is broken in any way, error core is returned
	 */
	uint8_t setParitalConfig(uint8_t seq, void *data, uint8_t ChunkSize);

	void setSensorSerialEventsMask(uint8_t mask) { mSerialEventsMask = mask; }

	uint8_t getSensorSerialEventsMask() const { return mSerialEventsMask; }

	/* make the sensor running */
	uint8_t Start();
	/* pause the sensor */
	uint8_t Pause();

   uint16_t GetMeasurementPeriod() const { return mMeasurementPeriod; }
   void TriggerMeasurement() { if (isRunning()) doTriggerMeasurement(); }
   bool isRunning() const { return (mState == SENSOR_RUNNING); }
   bool isConfigured() const { return (mState > SENSOR_CREATED); }
   uint8_t getSensorType() const { return mSensorType; }
   bool isMeasurementValid() const { return isRunning() && misMeasurementValid; }   // false if not triggered or measurement error
   uint8_t getSensorID() const { return mSensorID; }
   uint8_t getSensorApiVersion() const { return mApiVersion; }
   uint8_t getConfigBlobSize() const { return mConfigBlobSize; }
   uint8_t getMeasurementBlobSize() const { return mMeasurementBlobSize; }
   uint8_t *getConfigBlob() { return mConfigBlobPtr; }

   static tSensor* getSensor(uint8_t sensorID);

   static void Run();

#if CONFIG_SENSOR_SEND_EVENTS_USING_SERIAL
   /* send a serial frame with current measurement and SensorEventType as event type
    * if odDemand = false - frame will be sent only sensor is registered to such events
    *
    * if !isMeasurementValid() = event type will be EV_TYPE_MEASUREMENT_ERROR regardless of SensorEventType
    *
    */
   void sendSerialMsgSensorEvent(bool onDemand, uint8_t SensorEventType);
#endif //CONFIG_SENSOR_SEND_EVENTS_USING_SERIAL
protected:
   /* ApiVersion - the sensor may be located on remote node, and its version may not match the central node
    * For identification, API version must be increased every time the config OR the result data format changes
    */
   tSensor(uint8_t SensorType, uint8_t sensorID, uint8_t ApiVersion, uint8_t ConfigBlobSize, void *ConfigBlobPtr);

   void *mCurrentMeasurementBlob;
   uint8_t mMeasurementBlobSize;
   uint8_t mPartialConfigSeq;

   void onMeasurementCompleted(bool Status);

   virtual void doTriggerMeasurement() = 0;
   virtual uint8_t onSetConfig() { return STATUS_SUCCESS; }
   virtual uint8_t onRun() { return STATUS_SUCCESS; }
   virtual uint8_t onPause() { return STATUS_SUCCESS; }

   // called every SENSOR_PROCESS_SERVICE_TIME
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
