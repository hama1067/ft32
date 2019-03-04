/*
 * ConfigHandler.cpp
 *
 *  Created on: Feb 28, 2019
 *      Author: joseph
 */

#include "ConfigHandler.h"

ConfigHandler::ConfigHandler(SHM * pSHM) {
	ptrSHM = pSHM;
}

ConfigHandler::~ConfigHandler() {
	delete ptrSHM;
}

