/*
 * tTimestamp.h
 *
 *  Created on: Jan 3, 2024
 *      Author: mszkudli
 */

#pragma once

#include "../../global.h"

#if CONFIG_TIMESTAMP

class tTimestamp {
private:
    tTimestamp() {}   // no instance
    static uint16_t mTimestamp;
public:
    static uint16_t get() { return mTimestamp; }
    static void set(uint16_t timestamp) { mTimestamp = timestamp; }
};

#endif //CONFIG_TIMESTAMP
