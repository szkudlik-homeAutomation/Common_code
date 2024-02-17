/*
 * tWiegandSensor.cpp
 *
 *  Created on: 21 pa� 2022
 *      Author: szkud
 */

#include "../../../global.h"
#if CONFIG_WIEGAND_SENSOR

#include "tWiegandSensor.h"

uint8_t tWiegandSensor::onSetConfig()
{
	Wiegant.begin(Config.PinD0, Config.PinD1);
	return STATUS_SUCCESS;
}

void tWiegandSensor::doTriggerMeasurement()
{
	if (Wiegant.available())
	{
		mResult.code = Wiegant.getCode();
		mResult.type = Wiegant.getWiegandType();
		onMeasurementCompleted(true);
	}
	else
	{
		mResult.code = 0;
		mResult.type = 0;
	}
}

#endif //CONFIG_WIEGAND_SENSOR
