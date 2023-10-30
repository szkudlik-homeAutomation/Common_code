#pragma once


/*
 * NOTE - backward compatibility
 *
 * message types MUST stay as they are!!!
 * Light Control is already installed and cannot be easily changed
 *
 * light control uses IDs up to 0x11
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
