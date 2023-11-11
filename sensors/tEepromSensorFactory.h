/*
 * tEepromSensorFactory.h
 *
 *  Created on: Nov 7, 2023
 *      Author: mszkudli
 */

#pragma once

#include "../../../global.h"
#include "tSensor.h"

#if CONFIG_EEPROM_SENSORS

class tEepromSensor {
public:
    tEepromSensor() { FirstEntry(); };
    ~tEepromSensor() {};

    void FirstEntry();
    void FirstFreeEntry();

    /* create and configure sensor based on eeprom entry */
    tSensor *createSensor();

    /* save given sensor to eeprom at first free slot */
    uint8_t save(tSensor *pSensor);

    /* clear EEPROM, delete all entries */
    static void clear();

    /* switch to next eeprom entry return false if this was the last one */
    bool switchToNext();

    bool isEmptyEntry() { return Entry.ID == EMPTY_ENTRY_ID; }
private:
    void get() { EEPROM.get(Offset,Entry); }
    void put(void *pConfig) { EEPROM.put(Offset,Entry); }

    /* get a size of EEPROM structure */
    uint8_t size() { return sizeof(Entry) + Entry.ConfigSize;}

    tEepromSensorEepromEntry Entry;   // copy of eeprom entry, without sensor specific config
    uint16_t Offset;

};

class tEepromSensorFactory {
public:
    tEepromSensorFactory();

    void saveAll();
    void recallAll();

 };

#endif //CONFIG_EEPROM_SENSORS
