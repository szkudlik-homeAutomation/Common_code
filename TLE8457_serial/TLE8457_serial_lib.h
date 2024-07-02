#pragma once


#include "../../../global.h"
#if CONFIG_TLE8457_COMM_LIB


#include "CommonFramesDefs.h"

#define DEVICE_ID_BROADCAST 0xFF
#define MAX_NUM_OF_NODES 32

/**
 * PROTOCOL
 *
 * 1) choose new seq number (increase)
 * 2) send a frame
 *      listen for a clear wire
 *       CSMA/CR - listen to the wire and in case of incorrect reading stop transmission. The lower ID device should win
 * 3) wait random time - random seed is a device ID
 * 4) repeat steps 2-4 given number of times
 *
 * recieve:
 * 1) frame with incorrect CRC => reject
 * 2) check if pair SenderDevId/Seq was recently seen (keep a list of last pairs). If yes => reject
 * 3) execute
 */


/**
 * Communication frame
 */
typedef struct
{
  uint8_t SenderDevId;    // id of the sender
  uint8_t DstDevId;       // device ID the message is sent to or broadcast
  uint8_t Seq;            // seq number. Retransmitted frame should have the same seq
  uint8_t MessageType;    // MESSAGE_TYPE*

  uint8_t Data[CONFIG_COMMUNICATION_PAYLOAD_DATA_SIZE];  // data structure, to be mapped on tMessageType* structure

  uint16_t crc;   // CRC, frame will be rejected if CRC is incorrect
} tCommunicationFrame;

#include "CommSender.h"
#include "CommReciever.h"

#endif // CONFIG_TLE8457_COMM_LIB
