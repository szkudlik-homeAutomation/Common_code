#include "../../../global.h"
#if CONFIG_SENSOR_LOGGER

#include "tSensorLogger.h"

void tSensorLogger::onMessage(uint8_t type, uint16_t data, void *pData)
{
    if (type != MessageType_SensorEvent)
        return;

    uint8_t SensorID = data;
    if ((mSensorID != 0) && (data != mSensorID))
        return;

    tSensorEvent *pSensorEvent = (tSensorEvent *)pData;
    if ((mSensorType != 0) && (pSensorEvent->SensorType != mSensorType))
        return;

    onSensorEvent(SensorID, pSensorEvent->EventType, pSensorEvent->ApiVersion, pSensorEvent->dataBlobSize, pSensorEvent->pDataBlob);
}

void tSensorLoggerTxt::onSensorEvent(uint8_t SensorID, uint8_t EventType, uint8_t ApiVersion, uint8_t dataBlobSize, void *pDataBlob)
{
	uint8_t *pDataTable = pDataBlob;
	// general log
	LOG_PRINT("==>Sensor event, sensor ID:");
	LOG(print(SensorID));
	LOG_PRINT(" event type ");
	LOG(print(EventType));
	LOG_PRINT(" Api verson  ");
	LOG(println(ApiVersion));

	LOG_PRINT("payload data: ");
	for (uint8_t i = 0; i < dataBlobSize; i++)
		LOG(print(pDataTable[i], HEX));
	LOG(println());
}

#endif //CONFIG_SENSOR_LOGGER
