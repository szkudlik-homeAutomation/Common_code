#pragma once


/*
 * NOTE - backward compatibility
 *
 * message types MUST stay as they are!!!
 * Light Control is already installed and cannot be easily changed
 *
 * !!!! light control uses IDs up to 0x11 !!!!!! and it must stay this way
 *
 */

/**
 * dest node must response with MESSAGE_TYPE_FW_VERSION_RESPONSE  to the central node
 * no payload data
 */
#define MESSAGE_TYPE_FW_VERSION_REQUEST 0x09

/**
 * firmware version
 */
#define MESSAGE_TYPE_FW_VERSION_RESPONSE 0x0A
typedef struct
{
  uint8_t Major;
  uint8_t Minor;
  uint8_t Patch;
} tMessageTypeFwVesionResponse;
C_ASSERT(sizeof(tMessageTypeFwVesionResponse) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

/**
 * force the note to reset. May be sent as a broadcast
 */
#define MESSAGE_TYPE_FORCE_RESET 0x0E

/**
 * request the state of all outputs from a device
 * device should respoind with MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE
 *
 */
#define MESSAGE_TYPE_OVERVIEW_STATE_REQUEST 0x01


/**
 * Sent on request - actual state of all outputs
 * sent to the device requesting it by MESSAGE_TYPE_OVERVIEW_STATE_REQUEST
 */
#define MESSAGE_TYPE_OVERVIEW_STATE_RESPONSE 0x02
typedef struct
{
  uint8_t  PowerState;      // state of 8 outputs, 1 means it is on
  uint8_t  TimerState;      // state of 8 timers, 1 means the output is on but there's timer pending, no timer value here
} tMessageTypeOverviewStateResponse;
C_ASSERT(sizeof(tMessageTypeOverviewStateResponse) <= COMMUNICATION_PAYLOAD_DATA_SIZE);


/**
 * request the state of an ouput from a device
 * device should respoind with MESSAGE_TYPE_OUTPUT_STATE_REQUEST
 */
#define MESSAGE_TYPE_OUTPUT_STATE_REQUEST 0x03
typedef struct
{
  uint8_t  OutputID;      // id of an output
} tMessageTypeOutputStateRequest;


/**
 * Sent on request - actual state of an output
 * sent to the device requesting it by MESSAGE_TYPE_OUTPUT_STATE_RESPONSE
 */
#define MESSAGE_TYPE_OUTPUT_STATE_RESPONSE 0x04
typedef struct
{
  uint8_t  OutputID;        // id of an output
  uint8_t  PowerState;      // state - 1 means on
  uint16_t TimerValue;      // state of a timer. 0 means there's no timer pending
  uint16_t DefaultTimer;    // a default timer for the output.
} tMessageTypeOutputStateResponse;
C_ASSERT(sizeof(tMessageTypeOutputStateResponse) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

/**
 * Sent by the central node - arbitrary set state of a single output
 */
#define MESSAGE_TYPE_SET_OUTPUT 0x05
typedef struct
{
  uint8_t  OutId;        // the output id to be set
  uint8_t  State;        // state 0 or 1 to be set, where "1" means an active state
  uint16_t Timer;        // timer when the output should be turned off. In seconds, 0 means forever, 0xFFFF means default timer value
} tMessageTypeSetOutput;
C_ASSERT(sizeof(tMessageTypeSetOutput) <= COMMUNICATION_PAYLOAD_DATA_SIZE);


#define MESSAGE_TYPE_GENERAL_STATUS 0x12
typedef struct {
    uint8_t Status;
} tMesssageGeneralStatus;

#if CONFIG_SENSORS

/**
 * Look for a sensor with a given ID
 * A node that holds the sensor in question must response with MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE
 * usually send as broadcast
 */
#define MESSAGE_TYPE_GET_SENSOR_BY_ID_REQUEST 0x13
typedef struct
{
    uint8_t SensorID;
} tMessageGetSensorByIdReqest;
C_ASSERT(sizeof(tMessageGetSensorByIdReqest) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

#define MESSAGE_TYPE_GET_SENSOR_BY_ID_RESPONSE 0x14
typedef struct
{
    uint8_t SensorID;
    uint8_t ApiVersion;
    uint8_t SensorType;
    uint8_t isConfigured : 1,
			isRunning : 1,
			isMeasurementValid : 1,
			EventsMask		   : 4;
    uint16_t MeasurementPeriod;
    uint8_t ConfigBlobSize;
    uint8_t MeasurementBlobSize;
} tMessageGetSensorByIdResponse;
C_ASSERT(sizeof(tMessageGetSensorByIdResponse) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

/*
 * request for current sensor measurement
 * the node that has the sensor of given ID should respond with MESSAGE_TYPE_SENSOR_EVENT with "onDemand" bit set
 *
 * MESSAGE_TYPE_SENSOR_EVENT may be divided to several frames if needed
 */
#define MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST 0x15
typedef struct
{
    uint8_t SensorID;
} tMessageGetSensorMeasurementReqest;
C_ASSERT(sizeof(tMessageGetSensorMeasurementReqest) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

/*
 * an event from a sensor
 * sent either when a sensor reports a status or on demand
 */
#define MESSAGE_TYPE_SENSOR_EVENT 0x16
typedef struct
{
    uint8_t SensorID;
    uint8_t LastSegment  : 1,	// if "1" - no more segments
            EventType    : 4,   // SensorEventType (numeric)
			onDemand     : 1;	// this is a response for MESSAGE_TYPE_SENSOR_MEASUREMENT_REQUEST

    uint8_t SegmentSeq;				// if "0" - first segment, next segments must have SegmentSeq++
} tMessageSensorEventHeader;

#define SENSOR_MEASUREMENT_PAYLOAD_SIZE (COMMUNICATION_PAYLOAD_DATA_SIZE - sizeof(tMessageSensorEventHeader))

typedef struct
{
	tMessageSensorEventHeader Header;
	uint8_t Payload[SENSOR_MEASUREMENT_PAYLOAD_SIZE];
} tMessageSensorEvent;

C_ASSERT(sizeof(tMessageSensorEvent) == COMMUNICATION_PAYLOAD_DATA_SIZE);

/* Create a sensor
 * MESSAGE_TYPE_GENERAL_STATUS will be sent back
 */
#define MESSAGE_TYPE_SENSOR_CREATE 0x17
typedef struct
{
    uint8_t SensorID;
    uint8_t SensorType;
} tMessageSensorCreate;
C_ASSERT(sizeof(tMessageSensorCreate) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

/* send a config blob to remote sensor */
#define MESSAGE_TYPE_SENSOR_CONFIGURE 0x18
typedef struct
{
    uint8_t SensorID;
    uint8_t LastSegment  : 1,
    		SegmentSeq   : 7;
} tMessageSensorConfigureHeader;

typedef struct
{
	uint16_t MeasurementPeriod;
}tMessageSensorConfigureCommonData;

#define SENSOR_CONFIG_PAYLOAD_SIZE (COMMUNICATION_PAYLOAD_DATA_SIZE - sizeof(tMessageSensorConfigureHeader))

typedef struct
{
	tMessageSensorConfigureHeader Header;
	union {
		tMessageSensorConfigureCommonData Data;			// if LastSegment
		uint8_t Payload[SENSOR_CONFIG_PAYLOAD_SIZE];	// if !LastSegment
	};
} tMessageSensorConfigure;
C_ASSERT(sizeof(tMessageSensorConfigure) == COMMUNICATION_PAYLOAD_DATA_SIZE);

/* Start a sensor, set events mask
 * MESSAGE_TYPE_GENERAL_STATUS will be sent back
 */
#define MESSAGE_TYPE_SENSOR_START 0x19
typedef struct
{
    uint8_t SensorID;
    uint8_t SensorEventMask;	// bitmap
} tMessageSensorStart;
C_ASSERT(sizeof(tMessageSensorStart) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

/* Stop a sensor
 * MESSAGE_TYPE_GENERAL_STATUS will be sent back
 */
#define MESSAGE_TYPE_SENSOR_STOP 0x1A
typedef struct
{
    uint8_t SensorID;
} tMessageSensorStop;
C_ASSERT(sizeof(tMessageSensorStop) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

#endif //CONFIG_SENSORS
