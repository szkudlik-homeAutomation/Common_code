/*
 * tImpulseSensor.cpp
 *
 *  Created on: 16 pa� 2022
 *      Author: szkud
 */

#include "tImpulseSensor.h"
#include "../../lib/external/YetAnotherArduinoPcIntLibrary/src/YetAnotherPcInt.h"

#define WITHOUT_INTERRUPTION(CODE) {uint8_t sreg = SREG; noInterrupts(); {CODE} SREG = sreg;}

void PinIntHandler(void* pSensor, bool pinstate) {
	tImpulseSensor* pImpulseSensor = (tImpulseSensor*)pSensor;
	pImpulseSensor->Impulse();
  }


void tImpulseSensor::SetSpecificConfig(void *pBlob)
{
	CleanCnt();
	tConfig *pConfig = (tConfig*) pBlob;
	mContinousCnt = pConfig->ContinousCnt;

	pinMode(pConfig->Pin, INPUT_PULLUP);
    PcInt::attachInterrupt(pConfig->Pin, PinIntHandler, this, pConfig->mode);
    mCurrentMeasurementBlob = &mShadowCnt;
	mMeasurementBlobSize = sizeof(mShadowCnt);
}


void tImpulseSensor::doTriggerMeasurement()
{
	WITHOUT_INTERRUPTION(
			mShadowCnt = mCnt;
			if (! mContinousCnt) mCnt = 0;
			);
	onMeasurementCompleted(true);
}

void tImpulseSensor::CleanCnt()
{
	WITHOUT_INTERRUPTION(
			mCnt = 0;
			);
}
