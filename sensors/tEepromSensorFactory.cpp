/*
 * tEepromSensorFactory.cpp
 *
 *  Created on: Nov 7, 2023
 *      Author: mszkudli
 */


#include "../../../global.h"
#if CONFIG_EEPROM_SENSORS

#include "tEepromSensorFactory.h"

void tEepromSensor::FirstEntry()
{
	Offset = EEPROM_FIRST_SENSOR_ENTRY;
	get();
}

void tEepromSensor::FirstFreeEntry()
{
	FirstEntry();
	while (switchToNext());
	// assuming that there's a last entry in eeprom...
}

bool tEepromSensor::switchToNext()
{
	if (isEmptyEntry()) return false;
	Offset += size();
	get();
	return true;
}

void tEepromSensor::clear()
{
	  tEepromSensorEepromEntry EepromSensorEepromEntry;
	  EepromSensorEepromEntry.ID = EMPTY_ENTRY_ID;
	  EepromSensorEepromEntry.ConfigSize = 0;
	  EepromSensorEepromEntry.Flags = 0;
	  EepromSensorEepromEntry.SensorType = 0;

	  EEPROM.put(EEPROM_FIRST_SENSOR_ENTRY,EepromSensorEepromEntry);
 }

uint8_t tEepromSensor::save(tSensor *pSensor)
{
	if (!pSensor->isConfigured())
		return STATUS_SENSOR_INCORRECT_STATE;

	FirstFreeEntry();
	Entry.ID = pSensor->getSensorID();
	Entry.SensorType = pSensor->getSensorType();
	Entry.ApiVersion = pSensor->getSensorApiVersion();
}

tSensor *tEepromSensor::createSensor()
{
	if (isEmptyEntry())
		return NULL;

}

#endif //CONFIG_EEPROM_SENSORS
