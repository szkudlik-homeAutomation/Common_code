/*
 * debug.h
 *
 *  Created on: 10 sie 2022
 *      Author: szkud
 */

#pragma once

//TODO - move to app code, split

#include "../../global.h"

#if !CONFIG_LOGGER

#undef DEBUG_1_ENABLE
#undef DEBUG_2_ENABLE
#undef DEBUG_3_ENABLE

#define LOG_PRINTLN(x)
#define LOG_PRINT(x)
#define LOG(x)

#else // !CONFIG_LOGGER

#include "Logger.h"

#define LOG_PRINTLN(x) { tLogger::Instance->EnableLogsForce(); tLogger::Instance->println(F(x)); tLogger::Instance->DisableLogsForce(); }
#define LOG_PRINT(x)   { tLogger::Instance->EnableLogsForce(); tLogger::Instance->print(F(x)); tLogger::Instance->DisableLogsForce(); }
#define LOG(x) 		   { tLogger::Instance->EnableLogsForce(); tLogger::Instance->x; ; tLogger::Instance->DisableLogsForce(); }

#endif // !CONFIG_LOGGER

#ifdef DEBUG_1_ENABLE
#define DEBUG_PRINTLN_1(x) tLogger::Instance->println(F(x))
#define DEBUG_PRINT_1(x) tLogger::Instance->print(F(x))
#define DEBUG_1(x) tLogger::Instance->x
#else
#define DEBUG_PRINTLN_1(x)
#define DEBUG_PRINT_1(x)
#define DEBUG_1(x)
#endif

#ifdef DEBUG_2_ENABLE
#define DEBUG_PRINTLN_2(x) tLogger::Instance->println(F(x))
#define DEBUG_PRINT_2(x) tLogger::Instance->print(F(x))
#define DEBUG_2(x) tLogger::Instance->x
#else
#define DEBUG_PRINTLN_2(x)
#define DEBUG_PRINT_2(x)
#define DEBUG_2(x)
#endif

#ifdef DEBUG_3_ENABLE
#define DEBUG_PRINTLN_3(x) tLogger::Instance->println(F(x))
#define DEBUG_PRINT_3(x) tLogger::Instance->print(F(x))
#define DEBUG_3(x) tLogger::Instance->x
#else
#define DEBUG_PRINTLN_3(x)
#define DEBUG_PRINT_3(x)
#define DEBUG_3(x)
#endif
int getFreeRam();

