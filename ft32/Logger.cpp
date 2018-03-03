/*
 * Logger.cpp
 *
 *  Created on: Mar 1, 2018
 *      Author: joseph
 */

#include "Logger.h"

/* Null, because instance will be initialized on demand. */
Logger* Logger::instance = 0;

Logger* Logger::getInstance()
{
    if (instance == 0) {
        instance = new Logger();
    }

    return instance;
}

Logger::Logger() {
	mBaudrate = 115200;
}

Logger::~Logger() {
	// TODO Auto-generated destructor stub
}

