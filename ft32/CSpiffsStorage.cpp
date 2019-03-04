/*
 * CSpiffsStorage.cpp
 *
 *  Created on: 26.10.2018
 *      Author: tobias
 */

#include "CSpiffsStorage.h"

CSpiffsStorage::CSpiffsStorage()
{
  mLock = false;
}

CSpiffsStorage::~CSpiffsStorage(){};

bool CSpiffsStorage::save(String pData)
{
  if (mLock == true)
    return false;
  else
  {
    mLock = true;
    mSpiffs.writeFile(SPIFFS, "/spiffs-cody-storage.txt", pData);
    mLock = false;
    return true;
  }
}

bool CSpiffsStorage::append(String pData)
{
  if (mLock == true)
    return false;
  else
  {
    mLock = true;
    mSpiffs.appendFile(SPIFFS, "/spiffs-cody-storage.txt", pData);
    mLock = false;
    return true;
  }
}

bool CSpiffsStorage::getStr(String * pData)
{
  String ret;
  if (mLock == true)
    return false;
  else
  {
    mLock = true;
    *pData = mSpiffs.getFile(SPIFFS, "/spiffs-cody-storage.txt");
    mLock = false;
    return true;
  }
}

int CSpiffsStorage::getFileSize(const char * path) {
	return mSpiffs.getFileSize(SPIFFS, path);
}

bool CSpiffsStorage::fileExists(const char * path) {
	return mSpiffs.fileExists(SPIFFS, path);
}
