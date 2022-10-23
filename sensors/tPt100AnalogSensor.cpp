/*
 * tPt100AnalogSensor.cpp
 *
 *  Created on: 21 paŸ 2022
 *      Author: szkud
 */

#include "tPt100AnalogSensor.h"
#include "../../../global.h"

void tPt100AnalogSensor::doTriggerMeasurement()
{
   mResult.Temperature = analogRead(mPin);
   //TODO = convert to temperature

   onMeasurementCompleted(true);
}

void tPt100AnalogSensor::SetSpecificConfig(void *pBlob)
{
   tConfig *pConfig = (tConfig *)pBlob;
   mPin = pConfig->Pin;

   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);

   mConfigSet = true;
}
