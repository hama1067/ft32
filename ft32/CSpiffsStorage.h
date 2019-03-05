/*
 * CSpiffsStorage.h
 *
 *  Created on: 26.10.2018
 *      Author: tobias
 *
 *
 *      Class to Save Data permanetly on Spliffs to a designated .txt-File
 *
 *
 */

#ifndef CSPIFFSSTORAGE_H_
#define CSPIFFSSTORAGE_H_

#include "SPIFFSTest.h"

class CSpiffsStorage
{
public:
  CSpiffsStorage();
  ~CSpiffsStorage();
  bool save(String pData);
  bool append(String pData);
  bool getStr(String * pData);
  int getFileSize(const char * path);
  bool fileExists(const char * path);
  CSPIFFS getSpiffs();

private:
  bool mLock;
  CSPIFFS mSpiffs;
};

#endif /* CSPIFFSSTORAGE_H_ */
