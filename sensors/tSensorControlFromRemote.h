/*
 * tSensorControlFromRemote.h
 *
 *  Created on: 13 lut 2024
 *      Author: szkud
 */

#pragma	 once

#include "../../../global.h"

#if CONFIG_SENSOR_BASIC_REMOTE_CONTROL

#include "tSensor.h"
#include "../tMessageReciever.h"
#include "../TLE8457_serial/CommonFramesDefs.h"


class tSensorControlFromRemote: public tMessageReciever {
public:
	tSensorControlFromRemote()
	   { RegisterMessageType(MessageType_SerialFrameRecieved); }

protected:
    virtual void onMessage(uint8_t type, uint16_t data, void *pData);

private:
    void HandleMessageGetSensorByIdReqest(uint8_t sender, tMessageGetSensorByIdReqest *pFrame);
    void HandleMsgGetSensorMeasurementReqest(uint8_t SenderID, tMessageGetSensorMeasurementReqest *Message);
#if CONFIG_SENSOR_ADVANCED_REMOTE_CONTROL
    void HandleMsgSensorCreate(uint8_t sender, tMessageSensorCreate *pFrame);
    void HandleMsgSensorConfigure(uint8_t SenderID, tMessageSensorConfigure *Message);
    void HandleMsgSensorStart(uint8_t SenderID, tMessageSensorStart *Message);
    void HandleMsgSensorStop(uint8_t SenderID, tMessageSensorStop *Message);
#endif CONFIG_SENSOR_ADVANCED_REMOTE_CONTROL
#if CONFIG_SENSORS_STORE_IN_EEPROM_REMOTE_CONTROL
    void HandeMsgSaveSensorsToEeprom(uint8_t SenderID);
    void HandeMsgRestoreSensorsFromEeprom(uint8_t SenderID);
#endif CONFIG_SENSORS_STORE_IN_EEPROM_REMOTE_CONTROL
};

#endif /* CONFIG_SENSOR_BASIC_REMOTE_CONTROL */
