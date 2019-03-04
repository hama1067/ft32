/*
 * ConfigHandler.h
 *
 *  Created on: Feb 28, 2019
 *      Author: joseph
 */

#ifndef CONFIGHANDLER_H_
#define CONFIGHANDLER_H_

#include "ft_ESP32_SHM.h"

class ConfigHandler {
public:
	ConfigHandler(SHM * pSHM);
	virtual ~ConfigHandler();

	bool checkSpiffsStorageFile();
	bool checkNetworkConfigurationFile();


private:
	SHM *ptrSHM;
};

#endif /* CONFIGHANDLER_H_ */
