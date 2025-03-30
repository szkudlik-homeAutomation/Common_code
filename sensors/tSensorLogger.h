#pragma once

#include "../../../global.h"
#if CONFIG_SENSORS


#include "../tMessageReciever.h"


class tSensorLogger : public tMessageReciever
{
public:
    tSensorLogger(uint8_t SensorType) : mSensorType(SensorType), mSensorID(0), tMessageReciever()
        { RegisterMessageType(MessageType_SensorEvent); }

    tSensorLogger(uint8_t SensorType, uint8_t SensorID): mSensorType(SensorType), mSensorID(SensorID), tMessageReciever()
                { RegisterMessageType(MessageType_SensorEvent); }

    virtual void onMessage(uint8_t type, uint16_t data, void *pData);

protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob) = 0;

   uint8_t mSensorID;
   uint8_t mSensorType;
};

class tSensorLoggerTxt : public tSensorLogger
{
public:
	tSensorLoggerTxt(uint8_t SensorType) : tSensorLogger(SensorType) {}

	tSensorLoggerTxt(uint8_t SensorType, uint8_t SensorID): tSensorLogger(SensorType, SensorID) {}

protected:
    virtual void onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob);
};

#endif // CONFIG_SENSORS
