/*
 * debug.h
 *
 *  Created on: 10 sie 2022
 *      Author: szkud
 */

#ifndef SRC_DEBUG_H_
#define SRC_DEBUG_H_

//TODO - move to app code, split

#include "../../global.h"

#ifdef CONTROLLER

//#define DEBUG_1
//#define DEBUG_2
#define DEBUG_3

#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_EVENT serialEvent

#include "ResponseHandler.h"

#endif //ifdef CONTROLLER




#ifdef DEBUG_1
#define DEBUG_PRINTLN_1(x) RespHandler.println(F(x));
#else
#define DEBUG_PRINTLN_1(x)
#endif

#ifdef DEBUG_2
#define DEBUG_PRINTLN_2(x) RespHandler.println(F(x));
#else
#define DEBUG_PRINTLN_2(x)
#endif

#ifdef DEBUG_3
#define DEBUG_PRINTLN_3(x) RespHandler.println(F(x));
//#define DEBUG_PRINTLN_3(x) Serial.println(F(x));
#else
#define DEBUG_PRINTLN_3(x)
#endif




#endif /* SRC_DEBUG_H_ */
