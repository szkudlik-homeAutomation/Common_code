/*
 * helpers.h
 *
 *  Created on: Oct 19, 2022
 *      Author: mszkudli
 */

#ifndef SRC_COMMON_CODE_HELPERS_H_
#define SRC_COMMON_CODE_HELPERS_H_

#define ATOMIC(CODE) {uint8_t sreg = SREG; noInterrupts(); {CODE} SREG = sreg;}


#endif /* SRC_COMMON_CODE_HELPERS_H_ */
