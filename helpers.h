/*
 * helpers.h
 *
 *  Created on: Oct 19, 2022
 *      Author: mszkudli
 */

#pragma once

#define ATOMIC(CODE) {uint8_t sreg = SREG; noInterrupts(); {CODE} SREG = sreg;}

#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
