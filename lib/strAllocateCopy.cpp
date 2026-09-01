/*
 * stringCpy.cpp
 *
 *  Created on: 30 mar 2024
 *      Author: szkud
 */

#include "../../../global.h"
#include "strAllocateCopy.h"

char *strAllocateCopy(const char * source, uint8_t  sizeLimit)
{
	if (NULL == source)
		return NULL;
	size_t size = strnlen(source, sizeLimit);
	if (size > sizeLimit)
		size = sizeLimit;

	char *strCopy= malloc(size+1);	// 1 for zero delimiter
	if (NULL == strCopy)
		return NULL;
	strncpy(strCopy, source, size);
	*(strCopy + size) = 0;
	return strCopy;
}
