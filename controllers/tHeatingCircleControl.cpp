/*
 * tHeatingCircleControl.cpp
 *
 *  Created on: 11 pa� 2022
 *      Author: szkud
 */

#define DEBUG_2

#include "../../../global.h"
#if CONFIG_HEATING_CIRCLE_CONTROL

#include "tHeatingCircleControl.h"
#include "../sensors/tDS1820Sensor.h"


int16_t tHeatingCircleControl::mPumpStopTempThold = 0;
int16_t tHeatingCircleControl::mPumpStartTempThold = 0;

void tHeatingCircleControl::onEvent(uint8_t SensorID, tSensorEventType EventType, uint8_t dataBlobSize, void *pDataBlob)
{
   tDS1820Sensor::tResult *pDS1820Result =(tDS1820Sensor::tResult *) pDataBlob;

   if (mState == STATE_DISABLED)
   {
      // do nothing
      StopValve();
      PumpOff();
      return;
   }

   if (EventType == EV_TYPE_MEASUREMENT_ERROR)
	{
		StopValve();
		return;
	}
	if (EventType != EV_TYPE_MEASUREMENT_COMPLETED)
	{
		return;
	}

	if (mValveTempSensorDevID == 255)
	{
	   // find the sensor
	   for(uint8_t i = 0; i < pDS1820Result->NumOfDevices; i++)
	   {
	      if (strncmp(mValveTempSensorSerial,pDS1820Result->Dev[i].Addr,sizeof(tDS1820Sensor::DeviceAddress)) == 0)
	      {
	         mValveTempSensorDevID = i;
	      }
	      if (mHeatSourceSensorSerial)
            if (strncmp(mHeatSourceSensorSerial,pDS1820Result->Dev[i].Addr,sizeof(tDS1820Sensor::DeviceAddress)) == 0)
            {
               mHeatSourceSensorDevID = i;
            }
	      if (mHeatStorageSensorSerial)
            if (strncmp(mHeatStorageSensorSerial,pDS1820Result->Dev[i].Addr,sizeof(tDS1820Sensor::DeviceAddress)) == 0)
            {
               mHeatStorageSensorDevID = i;
            }
	   }
	}

	if (mValveTempSensorDevID == 255)
	{
      DEBUG_PRINT_3("Heating circle error - device not found");
      Disable();
      return;
	}

	int16_t CurrentTemperature = (pDS1820Result)->Dev[mValveTempSensorDevID].Temperature;
	int16_t Delta = abs(CurrentTemperature - mTargetTemp);
	bool doOpen = (mTargetTemp > CurrentTemperature);

	/*
	 * steps
	 *  - if disabled - do nothing
	 *  - if state is STATE_OFF - set valve to closing. STOP
	 *  - if Delta is bigger than mFastValveMoveThold - set the valve to opening/closing and set proper state (OPENING/CLOSING). STOP.
	 *  - if delta is smaller than mSlowValveTime:
	 *  	- if state is CLOSING and Temp is bigger than target ==> Set valve to close slow. STOP
	 *  	- if state is OPENING and Temp is smaller than target ==> Set valve to open slow. STOP
	 *  	- if state is IDLE and Delta is bigger than mTolerance - set the valve to opening/closing and set proper state (OPENING/CLOSING). STOP.
	 *
	 *  being here means - the temperature is OK,
	 *  	- stop the valve
	 *  	- set state to IDLE
	 */

	   ("-->Temp: ");
	   DEBUG_2(print((float)(CurrentTemperature) / 10));
	   DEBUG_PRINT_2(" Target: ");
	   DEBUG_2(print(getTargetTemp()));
	   DEBUG_PRINT_2(" Fast: ");
	   DEBUG_2(print(getFastThold()));
	   DEBUG_PRINT_2(" Tolerance: ");
	   DEBUG_2(print(getTolerance()));
      DEBUG_PRINT_2(" Histeresis: ");
      DEBUG_2(print(getHisteresis()));
      DEBUG_PRINT_2(" PumpStopTempThold: ");
      DEBUG_2(print(mPumpStopTempThold));
      DEBUG_PRINT_2(" PumpStartTempThold: ");
      DEBUG_2(println(mPumpStartTempThold));

	if (mState == STATE_OFF)
	{
		// pump is off, valve is closed
		CloseValve();
		PumpOff();
		return;
	}


	if ((mHeatSourceSensorDevID != 255) && (mHeatStorageSensorDevID != 255))
	{
	   // shoud we pause the pump?
	   int16_t HeatSourceTemperature = (pDS1820Result)->Dev[mHeatSourceSensorDevID].Temperature;
      int16_t HeatStorageTemperature = (pDS1820Result)->Dev[mHeatStorageSensorDevID].Temperature;

      if ((HeatSourceTemperature < mPumpStopTempThold) && (mPausePreventionCycles == 0))
      {
         mState = STATE_PAUSED;
      }
      if (mState == STATE_PAUSED)
	   {

	      if (HeatStorageTemperature > mPumpStartTempThold)
	      {
	         mState = STATE_IDLE;
	         mPausePreventionCycles = PAUSE_PREVENTION_CYCLES;
	      }
	      else
	      {
	         // do nothing - wait for heat source to warm up
	         DEBUG_PRINT_2("PAUSE ");
	         StopValve();
	         PumpOff();
	         return;
	      }
	   }
	}

	if (mPausePreventionCycles) { mPausePreventionCycles--; }

   PumpOn();

	if ( (Delta > mFastValveMoveThold) ||
		((mState == STATE_IDLE) && (Delta > mHisteresis)) )
	{
		// big diffrence in temp or was in idle and run out of tolerance
		// force valve to open/close
		if (doOpen)
		{
			OpenValve();
			mState = STATE_OPENING;
		}
		else
		{
			CloseValve();
			mState = STATE_CLOSING;
		}

		return;
	}

	// delta must be smaller than mFastValveMoveThold

	if (Delta > mTolerance)
	{
      if ((mState == STATE_OPENING) && (doOpen))
      {
         OpenValveSlow();
         return;
      }

      if ((mState == STATE_CLOSING) && (! doOpen))
      {
         CloseValveSlow();
         return;
      }
	}
	// all other cases - switch the valve off
	StopValve();
	mState = STATE_IDLE;
}
#endif //CONFIG_HEATING_CIRCLE_CONTROL
