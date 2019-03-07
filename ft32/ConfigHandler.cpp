/*
 * ConfigHandler.cpp
 *
 *  Created on: Feb 28, 2019
 *      Author: joseph
 */

#include "ConfigHandler.h"

ConfigHandler::ConfigHandler(SHM *ptr) {
  mPtr = ptr;
}

ConfigHandler::~ConfigHandler() {

}

void ConfigHandler::checkup() {

  bool spiffsCodyStorageFileExists = checkSpiffsCodyStorageFile();
  
  if(spiffsCodyStorageFileExists && mSpiffsStorage.getFileSize("/spiffs-cody-storage.txt") > 0) {
    Serial.println("[cfg] spiffs-cody-storage.txt found and not empty!");
    
    mSpiffsStorage.getStr(&(mPtr->webData.data));
    mPtr->webData.contentLength = mPtr->webData.data.length();
    
    Serial.println("[cfg] " + mPtr->webData.data);
  } else if (spiffsCodyStorageFileExists && mSpiffsStorage.getFileSize("/spiffs-cody-storage.txt") == 0) {
    Serial.println("[cfg] spiffs-cody-storage.txt found and empty! Preparing ...");
    mPtr->webData.data = ";";
    mPtr->webData.contentLength = 1;

    mSpiffsStorage.save(";");
  }
}

void ConfigHandler::setCipherKey(char * key) {
  cipher.setKey(key);
}

bool ConfigHandler::checkSpiffsCodyStorageFile() {
  bool returnCondition = mSpiffsStorage.fileExists("/spiffs-cody-storage.txt");
  
  #ifdef CONFIGHANDLER_DEBUG
    Serial.print("[cfg] Checking for /spiffs-cody-storage.txt");

    if(returnCondition) {
      Serial.println(" -> OK");
    } else {
      Serial.println(" -> FAIL");
    }
  #endif

  return returnCondition;
}

bool ConfigHandler::checkNetworkConfigurationFile() {
  bool returnCondition = mSpiffsStorage.fileExists("/wlanConfiguration.txt");
  
  #ifdef CONFIGHANDLER_DEBUG
    Serial.print("[cfg] Checking for /wlanConfiguration.txt");

    if(returnCondition) {
      Serial.println(" -> OK");
    } else {
      Serial.println(" -> FAIL");
    }
  #endif

  return returnCondition;
}

bool ConfigHandler::decipherNetworkConfigurationFile(String (& array) [2]) {  
  String cipheredConfigFileContent = mSpiffsStorage.getSpiffs().getFile(SPIFFS, "/wlanConfiguration.txt");
  String cipheredSsid = cipheredConfigFileContent.substring(0, cipheredConfigFileContent.indexOf('\n'));
  String cipheredPassword = cipheredConfigFileContent.substring(cipheredConfigFileContent.indexOf('\n') + 1);
  
  array[0] = cipher.decryptString(cipheredSsid);
  array[1] = cipher.decryptString(cipheredPassword);

  return true;
}

bool ConfigHandler::cipherNetworkConfigurationFile(String *configData) {
  bool returnCondition = false;
  
  String decipheredSsid = configData->substring(0, configData->indexOf('\n'));
  String decipheredPassword = configData->substring(configData->indexOf('\n') + 1);  
  String cipheredSsid = cipher.encryptString(decipheredSsid);
  String cipheredPassword = cipher.encryptString(decipheredPassword);

  String cipheredConfig = cipheredSsid + "\n" + cipheredPassword;

  #ifdef CONFIGHANDLER_DEBUG
    Serial.print("[cfg] configData: ");
    Serial.println(*configData);
    Serial.print("[cfg] cipher key: ");
    Serial.println(cipher.getKey());
    Serial.println("[cfg] deciphered ssid: " + decipheredSsid);
    Serial.println("[cfg] deciphered password: " + decipheredPassword);
    Serial.println("[cfg] ciphered ssid: " + cipheredSsid);
    Serial.println("[cfg] ciphered password: " + cipheredPassword);
    
    Serial.println("[cfg] deciphered ssid: " + cipher.decryptString(cipheredSsid));
    Serial.println("[cfg] deciphered password: " + cipher.decryptString(cipheredPassword));
  #endif

  if( checkNetworkConfigurationFile() ) {
    mSpiffsStorage.getSpiffs().writeFile(SPIFFS, "/wlanConfiguration.txt", cipheredConfig);
    returnCondition = true;
  } else {
    returnCondition = false;
  }
  
  return returnCondition;
}

bool ConfigHandler::checkWebFiles() {
  bool returnCondition = false;
  int errorCounter = 0;

  #ifdef CONFIGHANDLER_DEBUG
    Serial.println("[cfg] Checking for codypp web files ...");
  #endif 
  
  if( !mSpiffsStorage.fileExists("/index.html") ) {
    errorCounter++;

    #ifdef CONFIGHANDLER_DEBUG
      Serial.println("[cfg] Error: index.html is missing.");
    #endif
  }

  if( !mSpiffsStorage.fileExists("/main.css") ) {
    errorCounter++;

    #ifdef CONFIGHANDLER_DEBUG
      Serial.println("[cfg] Error: main.css is missing.");
    #endif
  }

  if( !mSpiffsStorage.fileExists("/main.js.gz") ) {
    errorCounter++;

    #ifdef CONFIGHANDLER_DEBUG
      Serial.println("[cfg] Error: main.js.gz is missing.");
    #endif
  }

  if( !mSpiffsStorage.fileExists("/manifest.js") ) {
    errorCounter++;  

    #ifdef CONFIGHANDLER_DEBUG
      Serial.println("[cfg] Error: manifest.js is missing.");
    #endif
  }

  if( !mSpiffsStorage.fileExists("/vendor.js.gz") ) {
    errorCounter++;

    #ifdef CONFIGHANDLER_DEBUG
      Serial.println("[cfg] Error: vendor.js.gz is missing.");
    #endif
  }

  if(errorCounter > 0) {
    returnCondition = false;
    
    #ifdef CONFIGHANDLER_DEBUG
      Serial.println("[cfg] -> FAIL");
    #endif
  } else {
    returnCondition = false;
    
    #ifdef CONFIGHANDLER_DEBUG
      Serial.println("[cfg] -> OK");
    #endif
  }

  return returnCondition;
}
