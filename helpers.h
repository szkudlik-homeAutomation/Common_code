/*
 * helpers.h
 *
 *  Created on: Oct 19, 2022
 *      Author: mszkudli
 */

#pragma once

#define BIT0 (1 << 0)
#define BIT1 (1 << 1)
#define BIT2 (1 << 2)
#define BIT3 (1 << 3)
#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT7 (1 << 7)


#define ATOMIC(CODE) {uint8_t sreg = SREG; noInterrupts(); {CODE} SREG = sreg;}

#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

#define stringify_literal( x ) # x
#define stringify_expanded( x ) stringify_literal( x )

#define CONCATENATE_FW_VERSION(A,B,C) stringify_expanded(A) "." stringify_expanded(B) "." stringify_expanded(C)

#define FW_VERSION CONCATENATE_FW_VERSION(FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH)
