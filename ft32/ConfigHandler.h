/*
 * ConfigHandler.h
 *
 *  Created on: Feb 28, 2019
 *      Author: joseph
 */

#ifndef CONFIGHANDLER_H_
#define CONFIGHANDLER_H_

#include "CSpiffsStorage.h"
#include "Cipher.h"
#include "ft_ESP32_SHM.h"

#define CONFIGHANDLER_DEBUG

class ConfigHandler {
public:
	ConfigHandler(SHM *ptr);
	virtual ~ConfigHandler();

  void checkup();
  void setCipherKey(char * key);
  
	bool checkSpiffsCodyStorageFile();
	bool checkNetworkConfigurationFile();
  bool decipherNetworkConfigurationFile(String (& array) [2]);
  bool checkWebFiles();

private:
  CSpiffsStorage mSpiffsStorage;
  Cipher cipher;
  SHM *mPtr;
};

#endif /* CONFIGHANDLER_H_ */
