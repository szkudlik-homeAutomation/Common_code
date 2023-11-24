/*
 * remoteSensorCreateTask.h
 *
 *  Created on: Nov 24, 2023
 *      Author: mszkudli
 */

#pragma once

#include "../WorkerProcess.h"
#include "../tMessageReciever.h"
#include "../tMessages.h"

class remoteSensorCreateTask: public WorkerTask, public tMessageReciever {
public:
    remoteSensorCreateTask()
    {
        RegisterMessageType(tMessages::MessageType_ExternalEvent);
    }
    virtual ~remoteSensorCreateTask() {}

};

