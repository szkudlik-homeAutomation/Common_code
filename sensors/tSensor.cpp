/*
 * tSensor.cpp
 *
 *  Created on: 28 sie 2022
 *      Author: szkud
 */

#include "../../../global.h"
#include "tSensor.h"

tSensor* tSensor::pFirst = NULL;

void tSensor::onMeasurementCompleted(bool Status)
{
   misMeasurementValid = Status;
   if (NULL != mSensorCallback)
      {
      if (Status)
         {
            mSensorCallback(this,EV_TYPE_MEASUREMENT_COMPLETED);
         }
         else
         {
            mSensorCallback(this,EV_TYPE_MEASUREMENT_ERROR);
         }
      }
}

void tSensor::Run()
{
   tSensor *i = pFirst;
   while(i != NULL)
   {
      // do the sensor job
      i->doTimeTick();

      // is it a time to trigger measurement?
      if (i->mMeasurementPeriod)
      {
         if (i->mCurrMeasurementPeriod == 0)
         {
            i->mCurrMeasurementPeriod = i->mMeasurementPeriod;
            i->TriggerMeasurement();
         }
         else
         {
            i->mCurrMeasurementPeriod--;
         }
      }
      i = i->pNext;
   }
}

void tSensorProcess::service()
{
   tSensor::Run();
}

void tSensorProcess::setup() {}
