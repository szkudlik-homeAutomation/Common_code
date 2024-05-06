/*
 * stringCpy.h
 *
 *  Created on: 30 mar 2024
 *      Author: szkud
 */

#pragma once

#include "../../../global.h"

/*
 * allocte memory for string and copy source to the allocated space
 * no more than sizeLimit +1 bytes will be allocated
 * result string does always have a null at the end
 *
 * @return a pointer to a copy of the string
 */
char *strAllocateCopy(const char * source, uint8_t  sizeLimit);
