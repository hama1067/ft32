/*
 * SPIFFS.cpp
 *
 *  Created on: Oct 23, 2017
 *      Author: vmuser
 */

#include "SPIFFSTest.h"

void CSPIFFS::listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
	Serial.printf("Listing directory: %s\n", dirname);

	File root = fs.open(dirname);
	if (!root) {
		Serial.println("Failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println("Not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels) {
				listDir(fs, file.name(), levels - 1);
			}
		} else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}

bool CSPIFFS::fileExists(fs::FS &fs, const char * path) {
  File file = fs.open(path);
  if (!(file && !file.isDirectory())) {
    //Serial.println("Failed to open file.");
    return false;
  } else {
    return true;
  }
}

void CSPIFFS::readFile(fs::FS &fs, const char * path) {
	Serial.printf("Reading file: %s\n", path);

	File file = fs.open(path);
	if (!file) {
		Serial.println("Failed to open file for reading");
		return;
	}

	Serial.print("Read from file: ");
	while (file.available()) {
		Serial.write(file.read());
	}
}

void CSPIFFS::writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
}

void CSPIFFS::appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
}

void CSPIFFS::renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void CSPIFFS::deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void CSPIFFS::testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

String CSPIFFS::getFile(fs::FS &fs, const char * path) {
  File file = fs.open(path);
  String output = "";
  
  for(int j = 0; j < file.size(); j++) {
    output += (char)file.read();
  }

  return output;
}

void CSPIFFS::printFileToClient(fs::FS &fs, const char * path, WiFiClient mClient) {
  File file = fs.open(path);
  constexpr int BUFF_SIZE=2048;
  char buffer[BUFF_SIZE];
  
  for(int block=0;block<file.size()/BUFF_SIZE;block++)
  {
      for(int j = 0; j <BUFF_SIZE-1; j++) {
        //mClient.print((char)file.read());
        buffer[j]=(char)file.read();
      }
      buffer[BUFF_SIZE-1]='\0';
      mClient.print(buffer);
  }
  for(int bytes_read=0;bytes_read<=file.size()%BUFF_SIZE+file.size()/BUFF_SIZE;bytes_read++)
  {
    buffer[bytes_read]=(char)file.read();
  }
  buffer[(file.size()%BUFF_SIZE)+file.size()/BUFF_SIZE]='\0';
  mClient.print(buffer);
}

void CSPIFFS::printFileToClient2(fs::FS &fs, const char * path, WiFiClient *mClient) {
  /*File file = fs.open(path);

  for(int j = 0; j < file.size(); j++) {
    mClient.print((char)file.read());
  }*/
  
  /*File file = fs.open(path);
  constexpr int BUFF_SIZE=2048;
  char buffer1[BUFF_SIZE];
  char buffer2[BUFF_SIZE];
  boolean writeFirst = true;
  
  for(int block=0;block<file.size()/BUFF_SIZE;block++)
  {
      for(int j = 0; j < BUFF_SIZE; j++) {
        //mClient.print((char)file.read());
        if (writeFirst) {
          buffer1[j]=(char)file.read();
        } else {
          buffer2[j] = (char)file.read();
        }
      }

      //::mClient.print(writeFirst ? buffer : buffer2);
      if(writeFirst) {
        mClient.print(buffer1);
      } else {
        mClient.print(buffer2);
      }

      writeFirst = ! writeFirst;
  }*/

  File file = fs.open(path);
  constexpr int BUFF_SIZE=2048;
  String buffer;
  
  for(int block=0;block<file.size()/BUFF_SIZE;block++) {
      for(int j = 0; j < BUFF_SIZE; j++) {
        //mClient.print((char)file.read());
        buffer += (char)file.read();
      }
      
      mClient->print(buffer);
      buffer = "";
  }

  buffer = "";
  
  for(int bytes_read=0;bytes_read<file.size()%BUFF_SIZE;bytes_read++) {
    buffer += (char)file.read();
  }
  
  mClient->print(buffer);
}

int CSPIFFS::getFileSize(fs::FS &fs, const char * path) {
  File file = fs.open(path);
  return file.size();
}

