/*
 * tHeatingCircleStatusSensor.cpp
 *
 *  Created on: 27 lis 2022
 *      Author: szkud
 */
#include "../../../global.h"
#if CONFIG_HEATING_CIRCLE_CONTROL_STATUS_SENSOR

#include "tHeatingCircleStatusSensor.h"


#include "../controllers/tHeatingCircleControl.h"


uint8_t tHeatingCircleStatusSensor::SetSpecificConfig(void *pBlob)
{
   tConfig *pConfig = (tConfig *)pBlob;
   mHeatingControl = pConfig->pHeatingControl;
   mCurrentMeasurementBlob = (void*) &mResult;
   mMeasurementBlobSize = sizeof(mResult);
   mConfigSet = true;
   return STATUS_SUCCESS;
}


void tHeatingCircleStatusSensor::doTriggerMeasurement()
{
   mResult.TargetTemp = mHeatingControl->getTargetTemp();
   mResult.isWorking = mHeatingControl->isWorking();
   onMeasurementCompleted(true);
}

uint8_t tHeatingCircleStatusSensor::TranslateBlobToJSON(uint8_t dataBlobSize, void *pDataCache, Stream *pStream)
{
   if (dataBlobSize != sizeof(tResult))
   {
         return STATUS_JSON_ENCODE_ERROR;
   }

   tResult *pResult =(tResult *) pDataCache;
   pStream->print(F("\"IsWorking\":"));
   pStream->print(pResult->isWorking);

   pStream->print(F(", \"TargetTemp\":"));
   pStream->print(pResult->TargetTemp);
   pStream->print(F(","));

   return STATUS_SUCCESS;
}
#endif
