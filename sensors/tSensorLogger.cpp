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

#endif //CONFIG_SENSORS
