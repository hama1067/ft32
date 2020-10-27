#ifndef __SPIFFSWeb__
#define __SPIFFSWeb__

#include "SPIFFS.h"
#include "FS.h"
#include <WiFi.h>
#include <Esp.h>

class CSPIFFS {
public:
  void listDir(fs::FS &fs, const char * dirname, uint8_t levels);             //Anzeigen der Dateien auf dem SPIFFS
  bool fileExists(fs::FS &fs, const char * path);                             //Teste ob File existiert
  void readFile(fs::FS &fs, const char * path);                               //Lesen der Dateien auf dem SPIFFS
  int writeFile(fs::FS &fs, const char * path, String message);              //Schreiben von Dateien auf dem SPIFFS
  void appendFile(fs::FS &fs, const char * path, String message);             //Anhängen von Dateiinhalten an ein bestehdens File im SPIFFS
  void renameFile(fs::FS &fs, const char * path1, const char * path2);        //Umbennen eines Files
  void deleteFile(fs::FS &fs, const char * path);                             //Löschen eines Files im SPIFFS
  void testFileIO(fs::FS &fs, const char * path);                             //Informationen über Lese- und Schreibprozess
  String getFile(fs::FS &fs, const char * path);                              //Get File as String directly
  void printFileToClient(fs::FS &fs, const char * path, WiFiClient mClient);  //Reading File and print it directly to client (for large files neccessary!)
  void printFileToClient2(fs::FS &fs, const char * path, WiFiClient *mClient);
  int getFileSize(fs::FS &fs, const char * path);

private:

};

#endif
