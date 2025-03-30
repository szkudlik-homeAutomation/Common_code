#include "../../../global.h"
#if CONFIG_SENSORS

#include "tSensorLogger.h"

void tSensorLogger::onMessage(uint8_t type, uint16_t data, void *pData)
{
    if (type != MessageType_SensorEvent)
        return;

    uint8_t SensorID = data;
    if ((mSensorID != 0) && (data != mSensorID))
        return;

    tSensorEvent *pSensorEvent = (tSensorEvent *)pData;
    if (pSensorEvent->SensorType != mSensorType)
        return;

    onSensorEvent(SensorID, pSensorEvent->EventType, pSensorEvent->dataBlobSize, pSensorEvent->pDataBlob);
}

void tSensorLoggerTxt::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t dataBlobSize, void *pDataBlob)
{
	uint8_t *pDataTable = pDataBlob;
	// general log
	LOG_PRINT("==>Sensor event, sensor ID:");
	LOG(print(SensorID));
	LOG_PRINT(" sensor type ");
	LOG(print(mSensorType));
	LOG_PRINT(" event type ");
	LOG(println(EventType));

	LOG_PRINT("payload data: ");
	for (uint8_t i = 0; i < dataBlobSize; i++)
		LOG(print(pDataTable[i], HEX));
	LOG(println());
}

#endif //CONFIG_SENSORS
