#pragma once


#include "../../../global.h"

#if CONFIG_TLE8457_COMM_LIB


#ifndef COMMUNICATION_PAYLOAD_DATA_SIZE
#define COMMUNICATION_PAYLOAD_DATA_SIZE 8
#endif

#ifndef COMM_SERIAL
#define COMM_SERIAL Serial
#endif

#ifndef COMM_SERIAL_EVENT
#define COMM_SERIAL_EVENT serialEvent
#endif

#ifndef DEVICE_ID_BROADCAST
#define DEVICE_ID_BROADCAST 0xFF
#endif

#ifndef NUM_OF_RETRANSMISSIONS
#define NUM_OF_RETRANSMISSIONS 1
#endif

#ifndef MAX_NUM_OF_RETRANSMISSIONS
#define MAX_NUM_OF_RETRANSMISSIONS 20
#endif

#ifndef FRAME_TRANSMISSION_TIME
#define FRAME_TRANSMISSION_TIME 10
#endif

#ifndef MAX_TRANSMIT_DELAY
#define MAX_TRANSMIT_DELAY 100
#endif

#ifndef OUTPUT_QUEUE_SIZE
#define OUTPUT_QUEUE_SIZE 3
#endif


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
 * 1) frame witj incorrect CRC => reject
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

  uint8_t Data[COMMUNICATION_PAYLOAD_DATA_SIZE];  // data structure, to be mapped on tMessageType* structure

  uint16_t crc;   // CRC, frame will be rejected if CRC is incorrect
} tCommunicationFrame;

#include "CommSender.h"
#include "CommReciever.h"

#endif // CONFIG_TLE8457_COMM_LIB
